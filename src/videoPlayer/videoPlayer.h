#pragma once

#include <SDL2/SDL.h>

#include "controls.h"
#include "../image/ImageStbi.h"

#include <time.h>
#include <stdio.h>

typedef struct {
  char * filepath;
  FILE * video_pipeline;
  size_t block_sz;
  double fps;
  ImageStbi * frame;
  
  // members for frame timing and seeking
  struct timespec video_start_time;
  size_t frame_count;

  // members for audio playback
  SDL_AudioSpec desired_spec;
  SDL_AudioDeviceID device;
  FILE * audio_pipeline;
  float volume;
  float muted_volume;
} VideoPlayer;

void play_video(const char *, const size_t);
// convert a mp4 video into a sequence of frames in "frames" folder
// and print them all out frame by frame in the terminal in ascii format