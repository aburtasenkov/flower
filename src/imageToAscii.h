#pragma once 

#include "grayscale.h"

#include <time.h>

typedef struct timespec timespec_t;

unsigned char rgbToGrayscale(unsigned char * pixel, int Ncomponents);
// convert rgb value of a pixel to an ascii char

char * createAsciiImage(const char * filename, unsigned char * image, int width, int height, int Ncomponents, int blockSize);
// create array of ascii chars for output
// returns NULL at error

void printImage(const char * filename, int blockSize);
// convert image located at filename into ascii character array and print it out in terminal

void sleepFrameTimeOffset(int FPS, timespec_t* start, timespec_t* end);
// sleep until the next frame should be displayed
// start and end are the times of the current frame processing

void printImageFPS(int FPS, const char * filename, int blockSize);
// wrapper print Image with FPS control using FPSWrapper function
