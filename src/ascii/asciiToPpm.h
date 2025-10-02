#pragma once

#include <stddef.h>
#include <stdint.h>

#define RGB_COLOR_CHANNELS 3

static size_t count_rows(const char *);
// return amount of y coordinates in ascii image format

static size_t count_columns(const char *);
// return amount of x coordinates in ascii image format

typedef struct {
    unsigned char * data;
    size_t x;
    size_t y;
} ImagePPM;

static ImagePPM * create_ppm(const size_t, const size_t);
// create object of ImagePPM class of size x * y

void free_ppm(ImagePPM *);

static void set_pixel(ImagePPM *, const size_t, const size_t, const uint8_t[RGB_COLOR_CHANNELS]);
// set pixel at (x,y) to rgb value

static ImagePPM * convert_ascii_to_ppm(const char *, const size_t, const size_t);

ImagePPM * ascii_to_ppm(const char *);