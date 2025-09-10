#pragma once 

#include "ImageStbi.h"

#include <stdint.h>

static unsigned char grayscaleToChar(uint8_t grayscaleValue);
// map a value of 0-255 on to an array of grayscale characters and return the character

static uint8_t rgbToGrayscale(const unsigned char * pixel, size_t Ncomponents);
// convert single or multichannel pixel to grayscale value [0:255]

char * stbiToAscii(const ImageStbi * stbi, size_t blockSize);
// create array of ascii characters
// this array includes \n and \o for newlines and end of string
// no checks and therefore the caller must ensure stbi is valid pointer

void printImage(const char * filepath, size_t blockSize);
// convert image located at filepath into ascii character array and print it out in terminal
