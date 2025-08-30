#include "grayscaleToPpm.h" 

#include "error.h"

#include <stdio.h>

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
}

void asciiToPpm(const char * ascii, const char * filepath) 
// write ascii characters in PPM file format to filepath
{
    if (!ascii) printCriticalError(ERROR_BAD_ARGUMENTS, "ascii is null pointer");
    if (!filename) printCriticalError(ERROR_BAD_ARGUMENTS, "filename is null pointer");

    File * ppm = fopen(filename);
    if (!ppm) printCriticalError(ERROR_BAD_ARGUMENTS, "Bad filepath [filename: %s]", filename);


    
}