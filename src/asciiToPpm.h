#pragma once

#include <stddef.h>
#include <stdint.h>

#define PADDING 10

#define RGB_ARRAY_SIZE 3

static size_t countRows(const char * ascii);
// return amount of y coordinates in ascii image format

static size_t countColumns(const char * ascii);
// return amount of x coordinates in ascii image format

typedef struct {
    unsigned char * data;
    size_t x;
    size_t y;
} ImagePPM;

static ImagePPM * createPPM(size_t x, size_t y);
// create object of ImagePPM class of size x * y

static void freePPM(ImagePPM * ppm);

static void setPixel(ImagePPM * ppm, size_t x, size_t y, const uint8_t rgb[RGB_ARRAY_SIZE]);
// set pixel at (x,y) to rgb value

static ImagePPM * convertAsciiToPpmBinary(const char * ascii, size_t asciiWidth, size_t asciiHeight);

void asciiToPpm(const char * ascii, const char * filepath);
// write ascii characters in PPM file format to filepath