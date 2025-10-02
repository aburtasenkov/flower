#pragma once 

#include "../image/ImageStbi.h"

#include <stdint.h>

void init_grayscale_ascii_map(void);
// create hash map for each value [0:255] to avoid recalculating ascii characters with same grayscale value

static inline char compute_grayscale_Char(const uint8_t);
// map a value of 0-255 on to an array of grayscale characters and return the character

static inline uint8_t rgb_grayscale_value(const unsigned char *, const size_t);
// convert single or multichannel pixel to grayscale value [0:255]

static inline char grayscale_char(const uint8_t);
// retrieve pre computed ascii characters

char * stbi_to_ascii(const ImageStbi *, const size_t);
// create array of ascii characters
// this array includes \n and \o for newlines and end of string
// no checks and therefore the caller must ensure stbi is valid pointer

void print_image(const char *, const size_t);
// convert image located at filepath into ascii character array and print it out in terminal
