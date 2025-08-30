#include "asciiToPpm.h" 

#include "error.h"
#include "Glyph.h"

#include <stdlib.h>
#include <stdio.h>

int countRows(const char * ascii) {
    if (!ascii) printCriticalError(ERROR_BAD_ARGUMENTS, "ascii is null pointer");

    int count = 0;
    while (*(ascii++) != '\0') {
        if (*ascii == '\n') ++count;
    }
    return ++count;
}

int countColumns(const char * ascii) {
    if (!ascii) printCriticalError(ERROR_BAD_ARGUMENTS, "ascii is null pointer");

    int count = 0;
    while (*(ascii++) != '\0') {
        if (*ascii == '\n') break; 
        ++count;
    }
    return ++count;
}

char * ppmHeader(const char * ascii)
// return PPM file header that will be written to PPM file
{
    if (!ascii) printCriticalError(ERROR_BAD_ARGUMENTS, "ascii is null pointer");

    // bool newLineReached = false;
    // int width = 0;
    // int height = 0;
    // while (*(ascii++) != '\0') {
    //     if (*ascii == '\n') {
    //         if (!newLineReached) {
    //             ++width;
    //             newLineReached = true;
    //         }
    //         ++height;
    //     }
    // }
    return NULL;
}

void setPixel(unsigned char * image, int rowSize, int x, int y, const uint8_t * rgb) {
    // pre conditions
    
    int index = (y * rowSize + x) * RGB_CHANNELS;
    image[index + 0] = rgb[0];
    image[index + 1] = rgb[1];
    image[index + 2] = rgb[2];
}

void asciiToPpm(const char * ascii, const char * filepath) 
// write ascii characters in PPM file format to filepath
{
    if (!ascii) printCriticalError(ERROR_BAD_ARGUMENTS, "ascii is null pointer");
    if (!filepath) printCriticalError(ERROR_BAD_ARGUMENTS, "filepath is null pointer");

    printf("%d\n", countRows(ascii));
    printf("%d\n", countColumns(ascii));

    int imageSizeX = GLYPH_W * countColumns(ascii);
    int imageSizeY = GLYPH_H  * countRows(ascii);
    int imageSize = imageSizeX * imageSizeY * RGB_CHANNELS;
    unsigned char * image = (unsigned char *)malloc(imageSize);

    // initialize all pixels to black
    for (int i = 0; i < imageSize; ++i) image[i] = 0;
}