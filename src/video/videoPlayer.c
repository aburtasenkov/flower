#include "videoPlayer.h"

#include "controls.h"
#include "videoData.h"
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

    if (read_frame(pipe, stbi) != stbi->data_sz) break;
    print_frame(stbi, block_sz, fps);
    ++current_frame;
  }

  // todo add handling for left and right arrows

  free_stbi(stbi);
  if (pipe) pclose(pipe);
  disable_raw_mode();
}
