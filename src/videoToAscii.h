#pragma once

#include <stdbool.h>

#include <time.h>

typedef struct timespec timespec_t;

int executeCommand(const char * command);
// Wrapper for system(*command*) calls in order to ease debugging

bool fileExists(const char * filename);

void printFrames(int blockSize, int FPS);
// print each frame of a video while accounting for wished FPS

void printVideo(const char * filename, int blockSize, int FPS);
// convert a mp4 video into a sequence of frames in "frames" folder
// and print them all out frame by frame in the terminal in ascii format

void sleepFrameTimeOffset(int FPS, timespec_t * start, timespec_t * end); 
// sleep until the next frame should be displayed
// start and end are the times of the current frame processing