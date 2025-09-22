#pragma once

#include "../image/ImageStbi.h"

#include <time.h>
#include <stdio.h>

static void sleep_frame_time_offset(const struct timespec * start, const struct timespec * end, const double FPS);
// sleep until the next frame should be displayed
// start and end are the times of the current frame processing

static void print_ui(const ImageStbi * stbi, const size_t block_sz);

static void print_frame(const ImageStbi * stbi,  const size_t block_sz, const size_t fps);

void play_video(const char * filepath, const size_t block_sz);
// convert a mp4 video into a sequence of frames in "frames" folder
// and print them all out frame by frame in the terminal in ascii format