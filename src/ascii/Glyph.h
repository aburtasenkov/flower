#pragma once

#include <stdint.h>
#include <stdlib.h>

#define GLYPH_W 8
#define GLYPH_H 8

// a glyph is 8x8 bits and therefore
#define GLYPH_MOST_SIGNIFICANT_BIT 0b10000000

const uint8_t * glyph_rows(const char);
// return binary rows of an ascii char for further image creation