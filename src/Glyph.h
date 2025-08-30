#pragma once

#include <stdint.h>
#include <stdlib.h>

#define GLYPH_W 5
#define GLYPH_H 7

extern uint8_t RGB_WHITE[];
extern uint8_t RGB_BLACK[];

#define RGB_CHANNELS 3

typedef struct { char c; uint8_t rows[GLYPH_H]; } Glyph;

extern const Glyph FONT[];
extern const size_t FONT_COUNT;

const uint8_t * glyphRows(char c);
// return binary rows of an ascii char for further image creation