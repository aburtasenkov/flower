#pragma once

#include <stddef.h>
#include <stdint.h>

int countRows(const char * ascii);
// return amount of y coordinates in ascii image format

int countColumns(const char * ascii);
// return amount of x coordinates in ascii image format

typedef struct {
    unsigned char * data;
    size_t x;
    size_t y;
} ImagePPM;

ImagePPM * createPPM(size_t x, size_t y);
// create object of ImagePPM class of size x * y

void freePPM(ImagePPM * ppm);

void setPixel(ImagePPM * ppm, int x, int y, const uint8_t * rgb);
// set pixel at (x,y) to rgb value

ImagePPM * convertAsciiToPpmBinary(const char * ascii, int asciiWidth, int asciiHeight);

void asciiToPpm(const char * ascii, const char * filepath);
// write ascii characters in PPM file format to filepath