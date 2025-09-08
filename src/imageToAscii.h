#pragma once 

#include "grayscale.h"

typedef struct {
    unsigned char * data;
    int width;
    int height;
    int Ncomponents;
} ImageStbi;

ImageStbi * loadStbi(const char * filename);
// create object of ImageStbi class that loads images in format that is supported by stbi

void freeStbi(ImageStbi * image);

unsigned char rgbToGrayscale(unsigned char * pixel, int Ncomponents);
// convert single or multichannel pixel to grayscale value [0:255]

char * createAsciiImage(const char * filename, unsigned char * image, int width, int height, int Ncomponents, int blockSize);
// create array of ascii characters
// this array includes \n and \o for newlines and end of string

void printImage(const char * filename, int blockSize);
// convert image located at filename into ascii character array and print it out in terminal

void printImageFPS(const char * filename, int blockSize, int FPS);
// wrap printing image with FPS control
