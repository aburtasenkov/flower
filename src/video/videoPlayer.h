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

void play_video(const char *, const size_t);
// convert a mp4 video into a sequence of frames in "frames" folder
// and print them all out frame by frame in the terminal in ascii format