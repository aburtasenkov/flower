#pragma once

#include <stdint.h>
#include <stdlib.h>

#define RGB_CHANNELS 3

extern const uint8_t RGB_WHITE[RGB_CHANNELS];
extern const uint8_t RGB_BLACK[RGB_CHANNELS];

#define GLYPH_W 8
#define GLYPH_H 8

// a glyph is 8x8 bits and therefore
#define GLYPH_MOST_SIGNIFICANT_BIT 0b10000000

typedef struct { 
  char c; 
  uint8_t rows[GLYPH_H]; 
} Glyph;

// font data
extern const Glyph FONT[];

const uint8_t * glyphRows(char c);
// return binary rows of an ascii char for further image creation