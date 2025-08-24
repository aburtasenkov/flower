#pragma once 

#include "grayscale.h"

#include <time.h>

typedef struct timespec timespec_t;

unsigned char rgbToGrayscale(unsigned char * pixel, int Ncomponents);
// convert single or multichannel pixel to grayscale value [0:255]

char * createAsciiImage(const char * filename, unsigned char * image, int width, int height, int Ncomponents, int blockSize);
// create array of ascii characters
// this array includes \n and \o for newlines and end of string

void printImage(const char * filename, int blockSize);
// convert image located at filename into ascii character array and print it out in terminal

void sleepFrameTimeOffset(int FPS, timespec_t* start, timespec_t* end);
// sleep until the next frame should be displayed
// start and end are the times of the current frame processing

void printImageFPS(const char * filename, int blockSize, int FPS);
// wrap printing image with FPS control
