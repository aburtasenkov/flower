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
#define SEEK_SECONDS 1

#define SAFE_CLOSE_PIPE(ptr) do { pclose(ptr); ptr = NULL; } while (0)

static VideoPlayer * create_VideoPlayer(const char * filepath, size_t block_sz) 
{
  if (!filepath) raise_critical_error(ERROR_BAD_ARGUMENTS, "filepath is null pointer");
  if (block_sz == 0) raise_critical_error(ERROR_BAD_ARGUMENTS, "block size is 0");

  VideoPlayer * video_player = (VideoPlayer *)malloc(sizeof(VideoPlayer));
  if (!video_player) raise_critical_error(ERROR_RUNTIME, "Could not allocate enough memory for object of class VideoPlayer");

  video_player->filepath = (char *)malloc(strlen(filepath) + 1);
  if (!video_player->filepath) 
  {
    free(video_player);
    raise_critical_error(ERROR_RUNTIME, "could not allocate enough memory for copy of video's filepath");
  }
  memcpy(video_player->filepath, filepath, strlen(filepath) + 1);

  video_player->data_pipeline = open_ffmpeg_pipeline(video_player->filepath, 0.0); // ffmpeg 3 byte image pipeline (R, G, B)
  
  video_player->block_sz = block_sz;
  {
    videoData video_data = get_video_information(video_player->filepath);
    video_player->fps = video_data.fps;
    video_player->frame = create_stbi(video_data.dimensions.width, video_data.dimensions.height, 3);
  }

  video_player->frame_count = 0;

  return video_player;
}

static void free_VideoPlayer(VideoPlayer * video_player)
{
  if (!video_player) raise_critical_error(ERROR_BAD_ARGUMENTS, "video_player is null pointer");

  if (video_player->filepath) free(video_player->filepath);
  if (video_player->data_pipeline) SAFE_CLOSE_PIPE(video_player->data_pipeline);
  if (video_player->frame) free_stbi(video_player->frame);

  free(video_player);
}

static void sleep_frame_time_offset(VideoPlayer * video_player, const struct timespec * start, const struct timespec * end) 
// sleep until the next frame should be displayed
// start and end are the times of the current frame processing
{
  double frame_time = 1.0 / video_player->fps;
  double elapsed_time = (end->tv_sec - start->tv_sec) + (end->tv_nsec - start->tv_nsec) / NANOSECONDS_IN_SECOND;
  double sleep_time = frame_time - elapsed_time;

  if (sleep_time > 0) {
    struct timespec sleep_duration;
    sleep_duration.tv_sec = (time_t)sleep_time;
    sleep_duration.tv_nsec = (long)((sleep_time - sleep_duration.tv_sec) * NANOSECONDS_IN_SECOND);
    nanosleep(&sleep_duration, NULL);
  }
}

static void print_ui(VideoPlayer * video_player)
{
  char * ascii_frame = stbi_to_ascii(video_player->frame, video_player->block_sz);

  printf("\033[H\033[J%s", ascii_frame);
  printf("[quit: q] [<-: left arrow] [play: space] [->: right arrow]\n");
  fflush(stdout);
  free(ascii_frame);
}

static void print_frame(VideoPlayer * video_player)
{
  struct timespec frame_start, frame_end;
  clock_gettime(CLOCK_MONOTONIC, &frame_start);

  print_ui(video_player);

  clock_gettime(CLOCK_MONOTONIC, &frame_end);

  sleep_frame_time_offset(video_player, &frame_start, &frame_end);
}

void seek_time(VideoPlayer * video_player, const double seconds)
// seek seconds backwards or forward
// if current timestamp if smaller than amount of frames that are seeked back - go back to 0.0 seconds
{
  int frame_diff = (int)(video_player->fps * seconds);
  printf("Seeking %d frames\n", frame_diff);

  if (frame_diff < 0 && video_player->frame_count < abs(frame_diff))
  {
    raise_noncritical_error(ERROR_EXTERNAL, "Cannot seek more frames back than current timestamp, going back to start of video");
    video_player->frame_count = 0;
  }
  else video_player->frame_count += frame_diff;

  if (video_player->data_pipeline) SAFE_CLOSE_PIPE(video_player->data_pipeline);
  video_player->data_pipeline = open_ffmpeg_pipeline(video_player->filepath, calculate_timestamp(video_player->frame_count, video_player->fps));

  size_t read_bytes = read_frame(video_player->data_pipeline, video_player->frame);
  if (read_bytes != video_player->frame->data_sz)
  {
    raise_noncritical_error(ERROR_EXTERNAL, "End of video or failed seek");
    ESCAPE_LOOP = true;
    return;
  }

  print_ui(video_player);
}

void play_video(const char * filepath, const size_t block_sz) {
  if (!filepath) raise_critical_error(ERROR_BAD_ARGUMENTS, "filepath is null pointer");
  if (block_sz < 1) raise_critical_error(ERROR_BAD_ARGUMENTS, "block_sz must be >= 1");

  VideoPlayer * video_player = create_VideoPlayer(filepath, block_sz);
  
  if (execute_command(CLEAR_COMMAND) != 0) 
  {
    disable_raw_mode();
    free_VideoPlayer(video_player);
    raise_critical_error(ERROR_RUNTIME, "Could not clean terminal");
  }
  
  enable_raw_mode();

  while (!ESCAPE_LOOP) 
  {
    check_keypress();
    
    // handle key presses
    if (MOVE_RIGHT) 
    {
      seek_time(video_player, SEEK_SECONDS);
      MOVE_RIGHT = false;
    }
    if (MOVE_LEFT) 
    {
      seek_time(video_player, -SEEK_SECONDS);
      MOVE_LEFT = false;
    }

    if (PAUSE)
    {
      while (PAUSE && !ESCAPE_LOOP)
      {
        check_keypress();
        usleep(SLEEP_ON_PAUSE_TIME);
      }

      if (ESCAPE_LOOP) break;
    }

    if (read_frame(video_player->data_pipeline, video_player->frame) != video_player->frame->data_sz) break;
    print_frame(video_player);
    ++video_player->frame_count;
  }

  disable_raw_mode();
  free_VideoPlayer(video_player);
}
