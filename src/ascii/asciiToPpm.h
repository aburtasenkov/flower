#pragma once

#include <stddef.h>
#include <stdint.h>

#define RGB_COLOR_CHANNELS 3

static size_t count_rows(const char * ascii);
// return amount of y coordinates in ascii image format

static size_t count_columns(const char * ascii);
// return amount of x coordinates in ascii image format

typedef struct {
    unsigned char * data;
    size_t x;
    size_t y;
} ImagePPM;

static ImagePPM * create_ppm(const size_t x, const size_t y);
// create object of ImagePPM class of size x * y

void free_ppm(ImagePPM * ppm);

static void set_pixel(ImagePPM * ppm, const size_t x, const size_t y, const uint8_t rgb[RGB_COLOR_CHANNELS]);
// set pixel at (x,y) to rgb value

static ImagePPM * convert_ascii_to_ppm(const char * ascii, const size_t ascii_width, const size_t ascii_height);

ImagePPM * ascii_to_ppm(const char * ascii);