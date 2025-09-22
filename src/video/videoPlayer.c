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

static char * VIDEO = NULL; 

static size_t BLOCK_SZ = 1;
static double FPS = 0;

static ImageStbi * FRAME = NULL;
static size_t FRAMECOUNT = 0;

static FILE * DATA_PIPELINE = NULL;

#define SEEK_SECONDS 1

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

void seek_time(const double seconds)
// seek seconds backwards or forward
// if current timestamp if smaller than amount of frames that are seeked back - go back to 0.0 seconds
{
  int frame_diff = (int)(FPS * seconds);
  printf("Seeking %d frames\n", frame_diff);

  if (frame_diff < 0 && FRAMECOUNT < abs(frame_diff))
  {
    raise_noncritical_error(ERROR_EXTERNAL, "Cannot seek more frames back than current timestamp, going back to start of video");
    FRAMECOUNT = 0;
  }
  else FRAMECOUNT += frame_diff;

  if (DATA_PIPELINE) pclose(DATA_PIPELINE);
  DATA_PIPELINE = open_ffmpeg_pipeline(VIDEO, calculate_timestamp(FRAMECOUNT, FPS));

  size_t read_bytes = read_frame(DATA_PIPELINE, FRAME);
  if (read_bytes != FRAME->data_sz)
  {
    raise_noncritical_error(ERROR_EXTERNAL, "End of video or failed seek");
    ESCAPE_LOOP = true;
    return;
  }

  print_ui();
}

static void main_loop()
{
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
        if (MOVE_RIGHT) 
        {
          seek_time(SEEK_SECONDS);
          MOVE_RIGHT = false;
        }
        if (MOVE_LEFT) 
        {
          seek_time(-SEEK_SECONDS);
          MOVE_LEFT = false;
        }
        usleep(SLEEP_ON_PAUSE_TIME);
      }

      if (ESCAPE_LOOP) break;

      DATA_PIPELINE = open_ffmpeg_pipeline(VIDEO, calculate_timestamp(FRAMECOUNT, FPS));
    }

    if (MOVE_RIGHT) 
    {
      seek_time(SEEK_SECONDS);
      MOVE_RIGHT = false;
    }
    if (MOVE_LEFT) 
    {
      seek_time(-SEEK_SECONDS);
      MOVE_LEFT = false;
    }

    if (read_frame(DATA_PIPELINE, FRAME) != FRAME->data_sz) break;
    print_frame();
    ++FRAMECOUNT;
  }

  disable_raw_mode();
}

void play_video(const char * filepath, const size_t block_sz) {
  if (!filepath) raise_critical_error(ERROR_BAD_ARGUMENTS, "filepath is null pointer");
  if (block_sz < 1) raise_critical_error(ERROR_BAD_ARGUMENTS, "block_sz must be >= 1");

  // prepare data for video player
  VIDEO = (char *)malloc(strlen(filepath) + 1);
  memcpy(VIDEO, filepath, strlen(filepath) + 1);
  BLOCK_SZ = block_sz;
  {
    videoData video_data = get_video_information(filepath);
    FRAME = create_stbi(video_data.dimensions.width, video_data.dimensions.height, 3);
    FPS = video_data.fps;
  }
  DATA_PIPELINE = open_ffmpeg_pipeline(filepath, 0.0); // ffmpeg 3 byte image pipeline (R, G, B)

  main_loop();

  // todo add handling for left and right arrows

  free_stbi(FRAME);
  if (DATA_PIPELINE) pclose(DATA_PIPELINE);
  free(VIDEO);
}
