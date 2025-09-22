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

static size_t BLOCK_SZ = 1;
static double FPS = 0;

static ImageStbi * FRAME = NULL;
static size_t FRAMECOUNT = 0;

static FILE * DATA_PIPELINE = NULL;

static void sleep_frame_time_offset(const struct timespec * start, const struct timespec * end) 
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

static void print_ui()
{
  char * ascii_frame = stbi_to_ascii(FRAME, BLOCK_SZ);

  printf("\033[H\033[J%s", ascii_frame);
  printf("[quit: q] [<-: left arrow] [play: space] [->: right arrow]\n");
  fflush(stdout);
  free(ascii_frame);
}

static void print_frame()
{
  struct timespec frame_start, frame_end;
  clock_gettime(CLOCK_MONOTONIC, &frame_start);

  print_ui(FRAME, BLOCK_SZ);

  clock_gettime(CLOCK_MONOTONIC, &frame_end);

  sleep_frame_time_offset(&frame_start, &frame_end);
}

void play_video(const char * filepath, const size_t block_sz) {
  if (!filepath) raise_critical_error(ERROR_BAD_ARGUMENTS, "filepath is null pointer");
  if (block_sz < 1) raise_critical_error(ERROR_BAD_ARGUMENTS, "block_sz must be >= 1");

  BLOCK_SZ = block_sz;
  {
    videoData video_data = get_video_information(filepath);
    FRAME = create_stbi(video_data.dimensions.width, video_data.dimensions.height, 3);
  }
  DATA_PIPELINE = open_ffmpeg_pipeline(filepath, 0.0); // ffmpeg 3 byte image pipeline (R, G, B)

  enable_raw_mode();

  while (true) 
  {
    check_keypress();
    
    // handle key presses
    if (ESCAPE_LOOP) break;
    if (PAUSE)
    {
      pclose(DATA_PIPELINE);
      DATA_PIPELINE = NULL;
      while (PAUSE && !ESCAPE_LOOP)
      {
        check_keypress();
        usleep(SLEEP_ON_PAUSE_TIME);
      }

      if (ESCAPE_LOOP) break;

      DATA_PIPELINE = open_ffmpeg_pipeline(filepath, calculate_timestamp(FRAMECOUNT, FPS));
    }

    if (read_frame(DATA_PIPELINE, FRAME) != FRAME->data_sz) break;
    print_frame(FRAME, BLOCK_SZ, FPS);
    ++FRAMECOUNT;
  }

  // todo add handling for left and right arrows

  free_stbi(FRAME);
  if (DATA_PIPELINE) pclose(DATA_PIPELINE);
  disable_raw_mode();
}
