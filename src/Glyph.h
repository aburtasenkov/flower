#pragma once

#include <stdint.h>
#include <stdlib.h>

#define RGB_CHANNELS 3

extern uint8_t RGB_WHITE[];
extern uint8_t RGB_BLACK[];

#define GLYPH_W 8
#define GLYPH_H 8

// a glyph is 8x8 bits and therefore
#define MOST_SIGNIFICANT_BIT 0b10000000

typedef struct { char c; uint8_t rows[GLYPH_H]; } Glyph;

// Each row uses the bytes to represent ascii chars in pixel color
extern const Glyph FONT[];
extern const size_t FONT_COUNT;


const uint8_t * glyphRows(char c);
// return binary rows of an ascii char for further image creation