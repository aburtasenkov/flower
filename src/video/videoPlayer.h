#pragma once

#include "../image/ImageStbi.h"

#include <time.h>
#include <stdio.h>

typedef struct {
  char * filepath;
  FILE * data_pipeline;
  size_t block_sz;
  double fps;
  ImageStbi * frame;
  size_t frame_count;
} VideoPlayer;

static VideoPlayer * create_VideoPlayer(const char * filepath, size_t block_sz);

static void free_VideoPlayer(VideoPlayer * video_player);

static void sleep_frame_time_offset(VideoPlayer * video_player, const struct timespec * start, const struct timespec * end);
// sleep until the next frame should be displayed
// start and end are the times of the current frame processing

static void print_ui(VideoPlayer * video_player);

static void print_frame(VideoPlayer * video_player);

void play_video(const char * filepath, const size_t block_sz);
// convert a mp4 video into a sequence of frames in "frames" folder
// and print them all out frame by frame in the terminal in ascii format