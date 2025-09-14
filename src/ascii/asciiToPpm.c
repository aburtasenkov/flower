#include "asciiToPpm.h" 

#include "../error.h"
#include "Glyph.h"

#include <stdlib.h>
#include <stdio.h>

// pixels of padding between characters
static const size_t PADDING = 0;

static const uint8_t RGB_BLACK[RGB_COLOR_CHANNELS] = {0, 0, 0};

static size_t count_rows(const char * ascii) 
// return amount of y coordinates in ascii image format
{
    size_t count = 1; // At least one row
    while (*ascii != '\0') {
        if (*ascii == '\n') ++count;
        ++ascii;
    }

    if (count < 1) printCriticalError(ERROR_RUNTIME, "Row count less than 1 [count: %zu]", count);

    return count - 1;
}

static size_t count_columns(const char * ascii)
// return amount of x coordinates in ascii image format
{
    size_t count = 0;
    while (*ascii != '\0' && *ascii != '\n') {
        ++count;
        ++ascii;
    }

    if (count < 1) printCriticalError(ERROR_RUNTIME, "Column count less than 1 [count: %zu]", count);

    return count;
}

static ImagePPM * create_ppm(size_t x, size_t y)
// create object of ImagePPM class
{
    if (x < 1 || y < 1) printCriticalError(ERROR_BAD_ARGUMENTS, "Invalid image dimensions [x: %zu, y: %zu]", x, y);

    size_t sz = x * y * RGB_COLOR_CHANNELS;

    ImagePPM * ppm = (ImagePPM *)malloc(sizeof(ImagePPM));
    if (!ppm) printCriticalError(ERROR_RUNTIME, "Cannot allocate enough memory for PPM Image [size in bytes: %zu]", sizeof(ImagePPM));

    ppm->data = (unsigned char *)malloc(sz);
    if (!ppm->data) printCriticalError(ERROR_RUNTIME, "Cannot allocate enough memory for PPM Image [size in bytes: %zu]", sz);
    ppm->x = x;
    ppm->y = y;

    return ppm;
}

void free_ppm(ImagePPM * ppm) {
    if (!ppm) return;

    free(ppm->data);
    free(ppm);
}

static void set_pixel(ImagePPM * ppm, size_t x, size_t y, const uint8_t rgb[RGB_COLOR_CHANNELS])
// set pixel at (x,y) to rgb value
{
    if (!rgb) printCriticalError(ERROR_BAD_ARGUMENTS, "rgb is null pointer");

    size_t index = (y * ppm->x + x) * RGB_COLOR_CHANNELS;
    size_t max_index = ppm->x * ppm->y * RGB_COLOR_CHANNELS;
    if (index + 2 >= max_index) printCriticalError(ERROR_RUNTIME, "Pixel index out of bounds [index: %zu, max: %zu]", index + 2, max_index);

    ppm->data[index + 0] = rgb[0];
    ppm->data[index + 1] = rgb[1];
    ppm->data[index + 2] = rgb[2];
}

static ImagePPM * convert_ascii_to_ppm(const char * ascii, size_t ascii_width, size_t ascii_height) {
    size_t image_width = GLYPH_W * ascii_width + (ascii_width - 1) * PADDING;
    size_t image_height = GLYPH_H  * ascii_height + (ascii_height - 1) * PADDING;
    size_t image_sz = image_width * image_height * RGB_COLOR_CHANNELS;
    ImagePPM * ppm = create_ppm(image_width, image_height);

    // initialize all pixels to white
    for (size_t i = 0; i < image_sz; ++i) ppm->data[i] = 255;

    // pixel on which the drawing "pen" currently is
    size_t pen_x = 0, pen_y = 0;

    // iterate over each character
    for (size_t i = 0; ascii[i] != '\0'; ++i) {
        // move "pen" to start of line
        if (ascii[i] == '\n') {
            pen_x = 0;
            pen_y += GLYPH_H + PADDING;
            continue;
        }

        // get current row of pixels and iterate over it
        const uint8_t* pixel_rows = glyph_rows(ascii[i]);

        for (size_t glyph_row = 0; glyph_row < GLYPH_H; ++glyph_row) {
            uint8_t pixels = pixel_rows[glyph_row];
            for (size_t glyph_column = 0; glyph_column < GLYPH_W; ++glyph_column) {
                if (pixels << glyph_column & GLYPH_MOST_SIGNIFICANT_BIT) {
                    set_pixel(ppm, pen_x + glyph_column, pen_y + glyph_row, RGB_BLACK);
                }
            }
        }
        pen_x += GLYPH_W + PADDING;
    }
    return ppm;
}

ImagePPM * ascii_to_ppm(const char * ascii) 
{
    if (!ascii) printCriticalError(ERROR_BAD_ARGUMENTS, "ascii is null pointer");

    ImagePPM * ppm = convert_ascii_to_ppm(ascii, count_columns(ascii), count_rows(ascii));
    return ppm;
}