#pragma once

#include "../image/ImageStbi.h"

#include <time.h>
#include <stdio.h>

static void get_video_resolution(ImageStbi * stbi, const char * filename);
// write video resolution into stbi->width and stbi->height using ffprobe

static double get_video_fps(const char * filename);
// get fps of a video using ffprobe

static FILE * open_ffmpeg_pipeline(const char * filepath, const double timestamp);
// open 3 bit ffpmeg frame pipeline 
// each bit is a rgb channel

static void sleep_frame_time_offset(const struct timespec * start, const struct timespec * end, double FPS);
// sleep until the next frame should be displayed
// start and end are the times of the current frame processing

static void print_average_fps(const struct timespec * start, const struct timespec * end, size_t total_fps);

static int get_keypress();
// return current key pressed

static void check_keypress();
// set flags on key presses

void play_video(const char * filepath, size_t block_sz);
// convert a mp4 video into a sequence of frames in "frames" folder
// and print them all out frame by frame in the terminal in ascii format