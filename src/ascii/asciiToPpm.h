#pragma once

#include <stddef.h>
#include <stdint.h>

#define RGB_COLOR_CHANNELS 3

typedef struct {
    unsigned char * data;
    size_t x;
    size_t y;
} ImagePPM;

void free_ppm(ImagePPM *);

ImagePPM * ascii_to_ppm(const char *);