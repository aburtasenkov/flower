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

static VideoPlayer * create_VideoPlayer(const char *, size_t);

static void free_VideoPlayer(VideoPlayer *);

static void sleep_frame_time_offset(VideoPlayer *, const struct timespec *, const struct timespec *);
// sleep until the next frame should be displayed
// start and end are the times of the current frame processing

static void print_ui(VideoPlayer *);

static void print_frame(VideoPlayer *);

void play_video(const char *, const size_t);
// convert a mp4 video into a sequence of frames in "frames" folder
// and print them all out frame by frame in the terminal in ascii format