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

#define FFPROBE_COMMAND "ffprobe -v error -select_streams v:0 -show_entries stream=width,height -of csv=p=0:s=x \"%s\""
#define FFMPEG_COMMAND "ffmpeg -loglevel quiet -i \"%s\" -f rawvideo -pix_fmt rgb24 -"

static void get_video_resolution(ImageStbi * stbi, const char * filename)
// write video resolution into stbi->width and stbi->height using ffprobe
{
  char command[1024];
  char buffer[128];

  snprintf(command, sizeof(command), FFPROBE_COMMAND, filename);

  FILE * pipe = popen(command, "r");
  if (!pipe)
  {
    raise_critical_error(ERROR_RUNTIME, "popen failed running ffprobe");
  }

  if (fgets(buffer, sizeof(buffer), pipe))
  {
    sscanf(buffer, "%zux%zu", &stbi->width, &stbi->height);
  }
  pclose(pipe);
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

static void sleep_frame_time_offset(const timespec_t * start, const timespec_t * end, size_t FPS) 
// sleep until the next frame should be displayed
// start and end are the times of the current frame processing
{
  double frame_time = 1.0 / FPS;
  double elapsed_time = (end->tv_sec - start->tv_sec) + (end->tv_nsec - start->tv_nsec) / NANOSECONDS_IN_SECOND;
  double sleep_time = frame_time - elapsed_time;

  if (sleep_time > 0) {
    timespec_t sleep_duration;
    sleep_duration.tv_sec = (time_t)sleep_time;
    sleep_duration.tv_nsec = (long)((sleep_time - sleep_duration.tv_sec) * NANOSECONDS_IN_SECOND);
    nanosleep(&sleep_duration, NULL);
  }
}

static void print_average_fps(const timespec_t * start, const timespec_t * end, size_t total_fps)
{
  double elapsed = (end->tv_sec - start->tv_sec) + (end->tv_nsec - start->tv_nsec) / 1e9;

  if (elapsed > 0.0) {
    double avg_fps = total_fps / elapsed;
    printf("\nAverage FPS: %.2f (%zu frames in %.3f seconds)\n",
            avg_fps, total_fps, elapsed);
  }
}

void print_video(const char * filepath, size_t block_sz, size_t FPS) {
  if (!filepath) raise_critical_error(ERROR_BAD_ARGUMENTS, "filepath is null pointer");
  if (block_sz < 1) raise_critical_error(ERROR_BAD_ARGUMENTS, "block_sz must be >= 1");

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

  // timesteps for managing FPS cap and counting average fps
  timespec_t frame_start, frame_end, start, end;
  size_t total_fps = 0;

  clock_gettime(CLOCK_MONOTONIC, &start);

  // read frames from pipeline, convert them into ascii art and print
  while (fread(buffer, 1, frame_sz, pipe) == frame_sz) {
    clock_gettime(CLOCK_MONOTONIC, &frame_start);

    char * ascii_frame = stbi_to_ascii(&stbi, block_sz);

    printf("\033[H\033[J%s", ascii_frame);
    fflush(stdout);

    free(ascii_frame);

    clock_gettime(CLOCK_MONOTONIC, &frame_end);
    sleep_frame_time_offset(&frame_start, &frame_end, FPS);
    total_fps++;
  }
  
  free(buffer);
  pclose(pipe);

  clock_gettime(CLOCK_MONOTONIC, &end);
  print_average_fps(&start, &end, total_fps);
}
