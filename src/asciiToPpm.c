#include "asciiToPpm.h" 

#include "error.h"
#include "Glyph.h"

#include <stdlib.h>
#include <stdio.h>

// pixels of padding between characters
static const size_t PADDING = 0;

// 3 8-bit integers for one color 
#define RGB_ARRAY_SIZE 3

static size_t countRows(const char * ascii) 
// return amount of y coordinates in ascii image format
{
    size_t count = 1; // At least one row
    while (*ascii != '\0') {
        if (*ascii == '\n') ++count;
        ++ascii;
    }

    if (count < 1) printCriticalError(ERROR_RUNTIME, "Row count less than 1 [count: %zu]", count);

    return count;
}

static size_t countColumns(const char * ascii)
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

static ImagePPM * createPPM(size_t x, size_t y)
// create object of ImagePPM class
{
    if (x < 1 || y < 1) printCriticalError(ERROR_BAD_ARGUMENTS, "Invalid image dimensions [x: %zu, y: %zu]", x, y);

    size_t sz = x * y * RGB_CHANNELS;

    ImagePPM * ppm = (ImagePPM *)malloc(sizeof(ImagePPM));
    if (!ppm) printCriticalError(ERROR_RUNTIME, "Cannot allocate enough memory for PPM Image [size in bytes: %zu]", sizeof(ImagePPM));

    ppm->data = (unsigned char *)malloc(sz);
    if (!ppm->data) printCriticalError(ERROR_RUNTIME, "Cannot allocate enough memory for PPM Image [size in bytes: %zu]", sz);
    ppm->x = x;
    ppm->y = y;

    return ppm;
}

static void freePPM(ImagePPM * ppm) {
    if (!ppm) return;

    free(ppm->data);
    free(ppm);
}

static void setPixel(ImagePPM * ppm, size_t x, size_t y, const uint8_t rgb[RGB_ARRAY_SIZE])
// set pixel at (x,y) to rgb value
{
    if (!rgb) printCriticalError(ERROR_BAD_ARGUMENTS, "rgb is null pointer");

    size_t index = (y * ppm->x + x) * RGB_CHANNELS;
    size_t max_index = ppm->x * ppm->y * RGB_CHANNELS;
    if (index + 2 >= max_index) printCriticalError(ERROR_RUNTIME, "Pixel index out of bounds [index: %zu, max: %zu]", index + 2, max_index);

    ppm->data[index + 0] = rgb[0];
    ppm->data[index + 1] = rgb[1];
    ppm->data[index + 2] = rgb[2];
}

static ImagePPM * convertAsciiToPpmBinary(const char * ascii, size_t asciiWidth, size_t asciiHeight) {
    size_t imageSizeX = GLYPH_W * asciiWidth + (asciiWidth - 1) * PADDING;
    size_t imageSizeY = GLYPH_H  * asciiHeight + (asciiHeight - 1) * PADDING;
    size_t imageSize = imageSizeX * imageSizeY * RGB_CHANNELS;
    ImagePPM * ppm = createPPM(imageSizeX, imageSizeY);

    // initialize all pixels to white
    for (size_t i = 0; i < imageSize; ++i) ppm->data[i] = 255;

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
        const uint8_t* pixelRows = glyphRows(ascii[i]);

        for (size_t glyphRow = 0; glyphRow < GLYPH_H; ++glyphRow) {
            uint8_t pixels = pixelRows[glyphRow];
            for (size_t glyphColumn = 0; glyphColumn < GLYPH_W; ++glyphColumn) {
                if (pixels << glyphColumn & GLYPH_MOST_SIGNIFICANT_BIT) {
                    setPixel(ppm, pen_x + glyphColumn, pen_y + glyphRow, RGB_BLACK);
                }
            }
        }
        pen_x += GLYPH_W + PADDING;
    }
    return ppm;
}

void asciiToPpm(const char * ascii, const char * filepath) 
// write ascii characters in PPM file format to filepath
{
    if (!ascii) printCriticalError(ERROR_BAD_ARGUMENTS, "ascii is null pointer");
    if (!filepath) printCriticalError(ERROR_BAD_ARGUMENTS, "filepath is null pointer");

    ImagePPM * ppm = convertAsciiToPpmBinary(ascii, countColumns(ascii), countRows(ascii));

    FILE *f = fopen(filepath, "wb");
    if (!f) {
        perror("fopen");
        freePPM(ppm);
        return;
    }

    fprintf(f, "P6\n%zu %zu\n255\n", ppm->x, ppm->y);
    if (fwrite(ppm->data, 1, ppm->x * ppm->y * RGB_CHANNELS, f) != ppm->x * ppm->y * RGB_CHANNELS) {
        perror("fwrite");
        fclose(f);
        freePPM(ppm);
        return;
    }

    if (fclose(f) != 0) {
        perror("fclose");
    }
    freePPM(ppm);
}