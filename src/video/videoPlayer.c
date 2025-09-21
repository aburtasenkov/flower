#include "videoPlayer.h"

#include "controls.h"
#include "../image/ImageStbi.h"
#include "../ascii/stbiToAscii.h"
#include "../io/files.h"
#include "../io/terminal.h"
#include "../error.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

#define SLEEP_ON_PAUSE_TIME 10000
#define NANOSECONDS_IN_SECOND 1e9
#define TIMESTAMP_ROUNDING_PRECISION 3

#define FFPROBE_RESOLUTION_COMMAND "ffprobe -v error -select_streams v:0 -show_entries stream=width,height -of csv=p=0:s=x \"%s\""
#define FFPROBE_FPS_COMMAND "ffprobe -v error -select_streams v:0 -show_entries stream=r_frame_rate -of default=noprint_wrappers=1:nokey=1 \"%s\""

#define FFMPEG_COMMAND "ffmpeg -loglevel quiet -ss %.3f -i \"%s\" -f rawvideo -pix_fmt rgb24 -"

static videoDimensions get_video_resolution(const char * filename)
// write video resolution into stbi->width and stbi->height using ffprobe
{
  char command[1024];
  char buffer[128];
  videoDimensions dimensions;

  snprintf(command, sizeof(command), FFPROBE_RESOLUTION_COMMAND, filename);

  FILE * pipe = popen(command, "r");
  if (!pipe)
  {
    raise_critical_error(ERROR_RUNTIME, "popen failed running ffprobe resolution command");
  }

  if (fgets(buffer, sizeof(buffer), pipe))
  {
    sscanf(buffer, "%zux%zu", &dimensions.width, &dimensions.height);
  }
  pclose(pipe);

  return dimensions;
}

static double get_video_fps(const char * filename)
// get fps of a video using ffprobe
{
  char command [1024];
  char buffer[64];
  int num, denom;
  double fps = 0;

  snprintf(command, sizeof(command), FFPROBE_FPS_COMMAND, filename);

  FILE * pipe = popen(command, "r");
  if (!pipe)
  {
    raise_critical_error(ERROR_RUNTIME, "popen failed running ffprobe fps command");
  }

  if (fgets(buffer, sizeof(buffer), pipe))
  {
    sscanf(buffer, "%d/%d", &num, &denom);
    if (num > 0 && denom > 0)
    {
      fps = (double)num / denom;
    }
  }

  pclose(pipe);
  
  return fps;
}

static double calculate_timestamp(const size_t current_frame, const double video_fps)
// return amount of seconds passed after current_frame shown frames
// return is floor divisioned by TIMESTAMP_ROUNDING_PRECISION
{
  double seconds = (double)current_frame / video_fps;
  double factor = pow(10.0, TIMESTAMP_ROUNDING_PRECISION);
  return round(seconds * factor) / factor;
}

static FILE * open_ffmpeg_pipeline(const char * filepath, const double timestamp)
// open 3 bit ffpmeg frame pipeline 
// each bit is a rgb channel
{
  char command[1024];
  snprintf(command, sizeof(command), FFMPEG_COMMAND, timestamp, filepath);

  FILE * pipe = popen(command, "r");
  if (!pipe) raise_critical_error(ERROR_RUNTIME, "popen failed running ffmpeg");

  return pipe;
}

static void sleep_frame_time_offset(const struct timespec * start, const struct timespec * end, const double FPS) 
// sleep until the next frame should be displayed
// start and end are the times of the current frame processing
{
  double frame_time = 1.0 / FPS;
  double elapsed_time = (end->tv_sec - start->tv_sec) + (end->tv_nsec - start->tv_nsec) / NANOSECONDS_IN_SECOND;
  double sleep_time = frame_time - elapsed_time;

  if (sleep_time > 0) {
    struct timespec sleep_duration;
    sleep_duration.tv_sec = (time_t)sleep_time;
    sleep_duration.tv_nsec = (long)((sleep_time - sleep_duration.tv_sec) * NANOSECONDS_IN_SECOND);
    nanosleep(&sleep_duration, NULL);
  }
}

static ImageStbi * create_frame(const char * filepath)
// create stbi object to read rgb frames into
{
  videoDimensions dimensions = get_video_resolution(filepath);
  ImageStbi * stbi = create_stbi(dimensions.width, dimensions.height, 3);

  stbi->data = (unsigned char *)malloc(stbi->data_sz);
  if (!stbi->data) raise_critical_error(ERROR_RUNTIME, "cannot allocate frame buffer");  

  return stbi;
}

static size_t read_frame(ImageStbi * stbi, FILE * pipe)
// return amount of bytes read from pipe
{
  return fread(stbi->data, 1, stbi->data_sz, pipe);
}

static void print_ui(const ImageStbi * stbi, const size_t block_sz)
{
  char * ascii_frame = stbi_to_ascii(stbi, block_sz);

  printf("\033[H\033[J%s", ascii_frame);
  printf("[quit: q] [<-: left arrow] [play: space] [->: right arrow]\n");
  fflush(stdout);
  free(ascii_frame);
}

static void print_frame(const ImageStbi * stbi,  const size_t block_sz, const size_t fps)
{
  struct timespec frame_start, frame_end;
  clock_gettime(CLOCK_MONOTONIC, &frame_start);

  print_ui(stbi, block_sz);

  clock_gettime(CLOCK_MONOTONIC, &frame_end);

  sleep_frame_time_offset(&frame_start, &frame_end, fps);
}

void play_video(const char * filepath, const size_t block_sz) {
  if (!filepath) raise_critical_error(ERROR_BAD_ARGUMENTS, "filepath is null pointer");
  if (block_sz < 1) raise_critical_error(ERROR_BAD_ARGUMENTS, "block_sz must be >= 1");

  double fps = get_video_fps(filepath);
  ImageStbi * stbi = create_frame(filepath);
  FILE * pipe = open_ffmpeg_pipeline(filepath, 0.0); // ffmpeg 3 byte image pipeline (R, G, B)
  size_t current_frame = 0; // timesteps for managing FPS cap

  enable_raw_mode();

  while (true) 
  {
    check_keypress();
    
    // handle key presses
    if (ESCAPE_LOOP) break;
    if (PAUSE)
    {
      pclose(pipe);
      pipe = NULL;
      while (PAUSE && !ESCAPE_LOOP)
      {
        check_keypress();
        usleep(SLEEP_ON_PAUSE_TIME);
      }

      if (ESCAPE_LOOP) break;

      pipe = open_ffmpeg_pipeline(filepath, calculate_timestamp(current_frame, fps));
    }

    if (read_frame(stbi, pipe) != stbi->data_sz) break;
    print_frame(stbi, block_sz, fps);
    ++current_frame;
  }

  // todo add handling for left and right arrows

  free_stbi(stbi);
  if (pipe) pclose(pipe);
  disable_raw_mode();
}
