#pragma once 

#include "grayscale.h"

typedef struct {
    unsigned char * data;
    int width;
    int height;
    int Ncomponents;
} ImageStbi;

ImageStbi * loadStbi(const char * filepath);
// create object of ImageStbi class that loads images in format that is supported by stbi

void freeStbi(ImageStbi * image);

unsigned char rgbToGrayscale(unsigned char * pixel, int Ncomponents);
// convert single or multichannel pixel to grayscale value [0:255]

char * stbiToAscii(ImageStbi * stbi, int blockSize);
// create array of ascii characters
// this array includes \n and \o for newlines and end of string

void printImage(const char * filepath, int blockSize);
// convert image located at filepath into ascii character array and print it out in terminal

void printImageFPS(const char * filepath, int blockSize, int FPS);
// wrap printing image with FPS control
