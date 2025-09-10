#pragma once

#include <stddef.h>

void writeImage(const char * filepath, const char * outputPath, size_t blockSize);
// convert image at filepath to ascii and write the resulting image in outputPath
// if file format is not supported, function will cause an error

static void writePpm(const char * filepath, const char * outputPath, size_t blockSize);
// convert image at filepath to ppm file

static void writeTxt(const char * filepath, const char * outputPath, size_t blockSize);
// convert image at filepath to txt file
