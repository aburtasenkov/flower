#pragma once

#include <time.h>

typedef struct timespec timespec_t;

static void get_video_resolution(ImageStbi * stbi, const char * filename);
// write video resolution into stbi->width and stbi->height using ffprobe

static FILE * open_ffmpeg_pipeline(const char * filepath);
// open 3 bit ffpmeg frame pipeline 
// each bit is a rgb channel

static void sleep_frame_time_offset(const timespec_t * start, const timespec_t * end, size_t FPS);
// sleep until the next frame should be displayed
// start and end are the times of the current frame processing

static void print_average_fps(const timespec_t * start, const timespec_t * end, size_t total_fps);

void print_video(const char * filepath, size_t block_sz, size_t FPS);
// convert a mp4 video into a sequence of frames in "frames" folder
// and print them all out frame by frame in the terminal in ascii format