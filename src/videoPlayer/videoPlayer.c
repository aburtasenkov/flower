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

typedef enum {
  PLAYER_STATE_PLAYING,
  PLAYER_STATE_PAUSED,
  PLAYER_STATE_EXITING
} PlayerState;

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

  video_player->data_pipeline = open_ffmpeg_video_pipeline(video_player->filepath, 0.0); // ffmpeg 3 byte image pipeline (R, G, B)
  
  video_player->block_sz = block_sz;
  {
    videoData video_data = get_video_information(video_player->filepath);
    video_player->fps = video_data.fps;
    video_player->frame = create_stbi(video_data.dimensions.width, video_data.dimensions.height, 3);
  }

  video_player->frame_count = 0;

  /*not initializing video_start_time member, as it should be initialized right before the video's start*/

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

static void sleep_until_next_frame(VideoPlayer * video_player)
// This function calculates the target wall-clock time for the next frame based on the
// video's start time and current frame count. This prevents cumulative timing errors.
{
  // total time that should've elapsed until the display of next frame
  double target_elapsed_seconds = (double)(video_player->frame_count + 1) / video_player->fps;

  // absolute wall-clock time for next frame's presentation
  struct timespec target_time;
  target_time.tv_sec = video_player->video_start_time.tv_sec + (time_t)target_elapsed_seconds;
  target_time.tv_nsec = video_player->video_start_time.tv_nsec + (long)((target_elapsed_seconds - (time_t)target_elapsed_seconds) * NANOSECONDS_IN_SECOND);

  // handle nanosecond overflow
  if (target_time.tv_nsec >= NANOSECONDS_IN_SECOND)
  {
    target_time.tv_sec++;
    target_time.tv_nsec -= (long)NANOSECONDS_IN_SECOND;
  }

  clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &target_time, NULL);
}

static void resync_video_start_time(VideoPlayer * video_player)
// resynchronize video_start time after disrupting event like a seek
// after seeking the video's start time is broken
// this function calculates a new, virtual start_time in the past, such that the
// current time correctly corresponds to the new frame_count
{
  struct timespec now;
  clock_gettime(CLOCK_MONOTONIC, &now);

  double current_offset_secs = (double)video_player->frame_count / video_player->fps;

  // calculate new start time in the past
  video_player->video_start_time.tv_sec = now.tv_sec - (time_t)current_offset_secs;
  video_player->video_start_time.tv_nsec = now.tv_nsec - (long)((current_offset_secs - (time_t)current_offset_secs) * NANOSECONDS_IN_SECOND);
  
  // handle nanosecond underflow
  if (video_player->video_start_time.tv_nsec < 0)
  {
    video_player->video_start_time.tv_sec--;
    video_player->video_start_time.tv_nsec += (long)NANOSECONDS_IN_SECOND;
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
  print_ui(video_player);

  sleep_until_next_frame(video_player);
}

static bool seek_time(VideoPlayer * video_player, const double seconds)
// seek seconds backwards or forward
// if current timestamp if smaller than amount of frames that are seeked back - go back to 0.0 seconds
// return true on successfull seek (accounts for seeking beyond the timeline)
// return false on failed read of the new frame
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
  video_player->data_pipeline = open_ffmpeg_video_pipeline(video_player->filepath, calculate_timestamp(video_player->frame_count, video_player->fps));

  size_t read_bytes = read_frame(video_player->data_pipeline, video_player->frame);
  if (read_bytes != video_player->frame->data_sz)
  {
    raise_noncritical_error(ERROR_EXTERNAL, "End of video or failed seek");
    return false;
  }

  resync_video_start_time(video_player);
  print_ui(video_player);
  return true;
}

static void update_state(VideoPlayer * video_player, UserInput * user_input, PlayerState * current_state)
{
  if (user_input->lowercase_q)
  {
    *current_state = PLAYER_STATE_EXITING;
    return;
  }

  switch (*current_state)
  {
    case PLAYER_STATE_PLAYING:
    {
      if (user_input->space)
      {
        *current_state = PLAYER_STATE_PAUSED;
        user_input->space = false;
      }
      else if (user_input->arrow_left)
      {
        if (!seek_time(video_player, -SEEK_SECONDS)) *current_state = PLAYER_STATE_EXITING;
        else *current_state = PLAYER_STATE_PAUSED;
        user_input->arrow_left = false;
      }
      else if (user_input->arrow_right)
      {
        if (!seek_time(video_player, SEEK_SECONDS)) *current_state = PLAYER_STATE_EXITING;
        else *current_state = PLAYER_STATE_PAUSED;
        user_input->arrow_right = false;
      }
      break;
    }
    case PLAYER_STATE_PAUSED:
    {
      if (user_input->space)
      {
        resync_video_start_time(video_player);
        *current_state = PLAYER_STATE_PLAYING;
        user_input->space = false;
      }
      else if (user_input->arrow_left)
      {
        if (!seek_time(video_player, -SEEK_SECONDS))
        {
          *current_state = PLAYER_STATE_EXITING;
        }
        user_input->arrow_left = false;
      }
      else if (user_input->arrow_right)
      {
        if (!seek_time(video_player, SEEK_SECONDS))
        {
          *current_state = PLAYER_STATE_EXITING;
        }
        user_input->arrow_right = false;
      }
      break;
    }
    case PLAYER_STATE_EXITING:
      break;
  }
}

void play_video(const char * filepath, const size_t block_sz) {
  if (!filepath) raise_critical_error(ERROR_BAD_ARGUMENTS, "filepath is null pointer");
  if (block_sz < 1) raise_critical_error(ERROR_BAD_ARGUMENTS, "block_sz must be >= 1");

  VideoPlayer * video_player = create_VideoPlayer(filepath, block_sz);
  UserInput user_input = {0};
  
  if (execute_command(CLEAR_COMMAND) != 0) 
  {
    disable_raw_mode();
    free_VideoPlayer(video_player);
    raise_critical_error(ERROR_RUNTIME, "Could not clean terminal");
  }

  enable_raw_mode();

  PlayerState current_state = PLAYER_STATE_PLAYING;
  clock_gettime(CLOCK_MONOTONIC, &video_player->video_start_time);
  while (current_state != PLAYER_STATE_EXITING) 
  {
    // perform actions for the current state
    switch (current_state)
    {
      case PLAYER_STATE_PLAYING:
      {
        if (read_frame(video_player->data_pipeline, video_player->frame) != video_player->frame->data_sz)
        {
          current_state = PLAYER_STATE_EXITING;
        }
        else
        {
          print_frame(video_player);
          ++video_player->frame_count;
        }
        break;
      }
      case PLAYER_STATE_PAUSED:
        usleep(SLEEP_ON_PAUSE_TIME);
        break;
      case PLAYER_STATE_EXITING:
        break;
    }

    // check for inputs and update state for the next loop iteration
    check_keypress(&user_input);
    update_state(video_player, &user_input, &current_state);
  }

  disable_raw_mode();
  free_VideoPlayer(video_player);
}
