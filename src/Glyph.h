#pragma once

#include <stdint.h>
#include <stdlib.h>

#define GLYPH_W 5
#define GLYPH_H 7

typedef struct { char c; uint8_t rows[GLYPH_H]; } Glyph;

extern const Glyph FONT[];
extern const size_t FONT_COUNT;