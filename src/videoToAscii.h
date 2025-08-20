#pragma once

#include "imageToAscii.h"

#include <stdbool.h>

#define FFMPEG_DECOMPOSE_VIDEO "ffmpeg -i %s -r 24 frames/frame_%%04d.png"
// --> max amount of frames = 9999
#define MAX_FRAMES 9999

// OS dependent command to clear the terminal
#ifdef _WIN32
const char * clearCommand = "cls";
#else
const char * clearCommand = "clear";
#endif

int executeCommand(const char * command);
// Wrapper for system(*command*) calls in order to ease debugging

bool fileExists(const char * filename);

void printFrames(int blockSize);

void printVideo(const char * filename, int blockSize);
// convert a mp4 video into a sequence of frames in "frames" folder
// and print them all out frame by frame in the terminal in ascii format