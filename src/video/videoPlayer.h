#pragma once

#include "controls.h"
#include "../image/ImageStbi.h"

#include <time.h>
#include <stdio.h>

typedef struct {
  char * filepath;
  FILE * data_pipeline;
  size_t block_sz;
  double fps;
  ImageStbi * frame;
  
  // members for frame timing and seeking
  struct timespec video_start_time;
  size_t frame_count;
} VideoPlayer;

static VideoPlayer * create_VideoPlayer(const char *, size_t);

static void free_VideoPlayer(VideoPlayer *);

static void sleep_until_next_frame(VideoPlayer * video_player);
// This function calculates the target wall-clock time for the next frame based on the
// video's start time and current frame count. This prevents cumulative timing errors.

static void resync_video_start_time(VideoPlayer * video_player);
// resynchronize video_start time after disrupting event like a seek
// after seeking the video's start time is broken
// this function calculates a new, virtual start_time in the past, such that the
// current time correctly corresponds to the new frame_count

static void print_ui(VideoPlayer *);

static void print_frame(VideoPlayer *);

static void seek_time(VideoPlayer * video_player, UserInput * user_input, const double seconds);
// seek seconds backwards or forward
// if current timestamp if smaller than amount of frames that are seeked back - go back to 0.0 seconds

void play_video(const char *, const size_t);
// convert a mp4 video into a sequence of frames in "frames" folder
// and print them all out frame by frame in the terminal in ascii format