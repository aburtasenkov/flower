#include "asciiToPpm.h" 

#include "error.h"
#include "Glyph.h"

#include <stdlib.h>
#include <stdio.h>

int countRows(const char * ascii) 
// return amount of y coordinates in ascii image format
{
    if (!ascii) printCriticalError(ERROR_BAD_ARGUMENTS, "ascii is null pointer");

    int count = 1; // At least one row
    while (*ascii != '\0') {
        if (*ascii == '\n') ++count;
        ++ascii;
    }

    if (count < 1) printCriticalError(ERROR_RUNTIME, "Row count less than 1 [count: %d]", count);

    return count;
}

int countColumns(const char * ascii)
// return amount of x coordinates in ascii image format
{
    if (!ascii) printCriticalError(ERROR_BAD_ARGUMENTS, "ascii is null pointer");

    int count = 0;
    while (*ascii != '\0' && *ascii != '\n') {
        ++count;
        ++ascii;
    }

    if (count < 1) printCriticalError(ERROR_RUNTIME, "Column count less than 1 [count: %d]", count);

    return count;
}

ImagePPM * createPPM(size_t x, size_t y)
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

void freePPM(ImagePPM * ppm) {
    if (!ppm) return;
    if (!ppm->data) { free(ppm); return; }

    free(ppm->data);
    free(ppm);
}

void setPixel(ImagePPM * ppm, int x, int y, const uint8_t * rgb)
// set pixel at (x,y) to rgb value
{
    if (!ppm) printCriticalError(ERROR_BAD_ARGUMENTS, "ppm is null pointer");
    if (!ppm->data) printCriticalError(ERROR_BAD_ARGUMENTS, "ppm->data is null pointer");
    if (!rgb) printCriticalError(ERROR_BAD_ARGUMENTS, "rgb is null pointer");
    if (ppm->x < 1 || ppm->y < 1) printCriticalError(ERROR_BAD_ARGUMENTS, "Invalid image dimensions");
    if (x < 0 || x >= (int)ppm->x) printCriticalError(ERROR_BAD_ARGUMENTS, "x coordinate out of bounds [x: %d, width: %zu]", x, ppm->x);
    if (y < 0 || y >= (int)ppm->y) printCriticalError(ERROR_BAD_ARGUMENTS, "y coordinate out of bounds [y: %d, height: %zu]", y, ppm->y);

    int index = (y * ppm->x + x) * RGB_CHANNELS;
    size_t max_index = ppm->x * ppm->y * RGB_CHANNELS;
    if (index < 0 || index + 2 >= (int)max_index) printCriticalError(ERROR_RUNTIME, "Pixel index out of bounds [index: %d, max: %zu]", index + 2, max_index);

    ppm->data[index + 0] = rgb[0];
    ppm->data[index + 1] = rgb[1];
    ppm->data[index + 2] = rgb[2];
}

ImagePPM * convertAsciiToPpmBinary(const char * ascii, int asciiWidth, int asciiHeight) {
    if (!ascii) printCriticalError(ERROR_BAD_ARGUMENTS, "ascii is null pointer");
    if (asciiWidth < 1 || asciiHeight < 1) printCriticalError(ERROR_BAD_ARGUMENTS, "Invalid ascii dimensions [asciiWidth: %d, asciiHeight: %d]", asciiWidth, asciiHeight);

    int imageSizeX = GLYPH_W * asciiWidth;
    int imageSizeY = GLYPH_H  * asciiHeight;
    int imageSize = imageSizeX * imageSizeY * RGB_CHANNELS;
    ImagePPM * ppm = createPPM(imageSizeX, imageSizeY);
    if (!ppm) printCriticalError(ERROR_RUNTIME, "Failed to create ppm image object");

    // initialize all pixels to white
    for (int i = 0; i < imageSize; ++i) ppm->data[i] = 255;

    // pixel on which the drawing "pen" currently is
    int pen_x = 0;
    int pen_y = 0;

    // iterate over each character
    for (int i = 0; ascii[i] != '\0'; ++i) {
        // move "pen" to start of line
        if (ascii[i] == '\n') {
            pen_x = 0;
            pen_y += GLYPH_H;
            continue;
        }

        // get current row of pixels and iterate over it
        const uint8_t* pixelRows = glyphRows(ascii[i]);

        for (int glyphRow = 0; glyphRow < GLYPH_H; ++glyphRow) {
            uint8_t pixels = pixelRows[glyphRow];
            for (int glyphColumn = 0; glyphColumn < GLYPH_W; ++glyphColumn) {
                if (pixels << glyphColumn & MOST_SIGNIFICANT_BIT) {
                    setPixel(ppm, pen_x + glyphColumn, pen_y + glyphRow, RGB_BLACK);
                }
            }
        }
        pen_x += GLYPH_W;
    }
    return ppm;
}

void asciiToPpm(const char * ascii, const char * filepath) 
// write ascii characters in PPM file format to filepath
{
    if (!ascii) printCriticalError(ERROR_BAD_ARGUMENTS, "ascii is null pointer");
    if (!filepath) printCriticalError(ERROR_BAD_ARGUMENTS, "filepath is null pointer");

    ImagePPM * ppm = convertAsciiToPpmBinary(ascii, countColumns(ascii), countRows(ascii));
    if (!ppm) printCriticalError(ERROR_RUNTIME, "convertAsciiToPpmBinary returned null pointer");

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

    fclose(f);
    freePPM(ppm);
}