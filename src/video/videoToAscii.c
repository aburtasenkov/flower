#include "videoToAscii.h"

#include "../image/ImageStbi.h"
#include "../ascii/stbiToAscii.h"
#include "../io/files.h"
#include "../io/terminal.h"

#include "../error.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define NANOSECONDS_IN_SECOND 1e9

#define FFPROBE_RESOLUTION_COMMAND "ffprobe -v error -select_streams v:0 -show_entries stream=width,height -of csv=p=0:s=x \"%s\""
#define FFPROBE_FPS_COMMAND "ffprobe -v error -select_streams v:0 -show_entries stream=r_frame_rate -of default=noprint_wrappers=1:nokey=1 \"%s\""

#define FFMPEG_COMMAND "ffmpeg -loglevel quiet -i \"%s\" -f rawvideo -pix_fmt rgb24 -"

static void get_video_resolution(ImageStbi * stbi, const char * filename)
// write video resolution into stbi->width and stbi->height using ffprobe
{
  char command[1024];
  char buffer[128];

  snprintf(command, sizeof(command), FFPROBE_RESOLUTION_COMMAND, filename);

  FILE * pipe = popen(command, "r");
  if (!pipe)
  {
    raise_critical_error(ERROR_RUNTIME, "popen failed running ffprobe resolution command");
  }

  if (fgets(buffer, sizeof(buffer), pipe))
  {
    sscanf(buffer, "%zux%zu", &stbi->width, &stbi->height);
  }
  pclose(pipe);
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

static FILE * open_ffmpeg_pipeline(const char * filepath)
// open 3 bit ffpmeg frame pipeline 
// each bit is a rgb channel
{
  char command[1024];
  snprintf(command, sizeof(command), FFMPEG_COMMAND, filepath);

  FILE * pipe = popen(command, "r");
  if (!pipe) raise_critical_error(ERROR_RUNTIME, "popen failed running ffmpeg");

  return pipe;
}

static void sleep_frame_time_offset(const struct timespec * start, const struct timespec * end, double FPS) 
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

static void print_average_fps(const struct timespec * start, const struct timespec * end, size_t total_fps)
{
  double elapsed = (end->tv_sec - start->tv_sec) + (end->tv_nsec - start->tv_nsec) / 1e9;

  if (elapsed > 0.0) {
    double avg_fps = total_fps / elapsed;
    printf("\nAverage FPS: %.2f (%zu frames in %.3f seconds)\n",
            avg_fps, total_fps, elapsed);
  }
}

void print_video(const char * filepath, size_t block_sz) {
  if (!filepath) raise_critical_error(ERROR_BAD_ARGUMENTS, "filepath is null pointer");
  if (block_sz < 1) raise_critical_error(ERROR_BAD_ARGUMENTS, "block_sz must be >= 1");

  double FPS = get_video_fps(filepath);

  // create stbi object to read into
  ImageStbi stbi;
  stbi.n_components = 3;
  get_video_resolution(&stbi, filepath);
  
  size_t frame_sz = stbi.width * stbi.height * 3;
  unsigned char * buffer = (unsigned char *)malloc(frame_sz);
  if (!buffer) raise_critical_error(ERROR_RUNTIME, "cannot allocate frame buffer");  
  stbi.data = buffer;

  // ffmpeg 3 byte image pipeline (R, G, B)
  FILE * pipe = open_ffmpeg_pipeline(filepath);

  // timesteps for managing FPS cap
  struct timespec frame_start, frame_end;

  // read frames from pipeline, convert them into ascii art and print
  while (fread(buffer, 1, frame_sz, pipe) == frame_sz) {
    clock_gettime(CLOCK_MONOTONIC, &frame_start);

    char * ascii_frame = stbi_to_ascii(&stbi, block_sz);

    printf("\033[H\033[J%s", ascii_frame);
    fflush(stdout);

    free(ascii_frame);

    clock_gettime(CLOCK_MONOTONIC, &frame_end);
    sleep_frame_time_offset(&frame_start, &frame_end, FPS);
  }
  
  free(buffer);
  pclose(pipe);
}
