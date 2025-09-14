#pragma once 

#include "../image/ImageStbi.h"

#include <stdint.h>

void init_grayscale_ascii_map();
// create hash map for each value [0:255] to avoid recalculating ascii characters with same grayscale value

static inline char compute_grayscale_Char(uint8_t grayscale);
// map a value of 0-255 on to an array of grayscale characters and return the character

static inline uint8_t rgb_grayscale_value(const unsigned char * pixel, size_t n_components);
// convert single or multichannel pixel to grayscale value [0:255]

static inline char grayscale_char(uint8_t grayscale);
// retrieve pre computed ascii characters

char * stbi_to_ascii(const ImageStbi * stbi, size_t block_sz);
// create array of ascii characters
// this array includes \n and \o for newlines and end of string
// no checks and therefore the caller must ensure stbi is valid pointer

void print_image(const char * filepath, size_t block_sz);
// convert image located at filepath into ascii character array and print it out in terminal
