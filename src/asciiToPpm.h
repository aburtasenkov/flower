#pragma once

char * ppmHeader(const char * ascii);
// return PPM file header that will be written to PPM file

void asciiToPpm(const char * ascii, const char * filepath);
// write ascii characters in PPM file format to filepath