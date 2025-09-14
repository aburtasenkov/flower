#pragma once

#include <stdbool.h>

#include <time.h>

typedef struct timespec timespec_t;

static int execute_command(const char * command);
// Wrapper for system(*command*) calls in order to ease debugging

static bool file_exists(const char * filepath);

static void print_frames(size_t block_sz, size_t FPS);
// print each frame of a video while accounting for wished FPS

void print_video(const char * filepath, size_t block_sz, size_t FPS);
// convert a mp4 video into a sequence of frames in "frames" folder
// and print them all out frame by frame in the terminal in ascii format

static void sleep_frame_time_offset(size_t FPS, const timespec_t * start, const timespec_t * end); 
// sleep until the next frame should be displayed
// start and end are the times of the current frame processing