#pragma once

#include <stdbool.h>

int executeCommand(const char * command);
// Wrapper for system(*command*) calls in order to ease debugging

bool fileExists(const char * filename);

void printFrames(int blockSize);

void printVideo(const char * filename, int blockSize);
// convert a mp4 video into a sequence of frames in "frames" folder
// and print them all out frame by frame in the terminal in ascii format