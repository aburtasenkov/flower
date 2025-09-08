#pragma once

void writeImage(const char * filepath, const char * outputPath, int blockSize);
// convert image at filepath to ascii and write the resulting image in outputPath
// if file format is not supported, function will cause an error

void writePpm(const char * filepath, const char * outputPath, int blockSize);
// convert image at filepath to ppm file

void writeTxt(const char * filepath, const char * outputPath, int blockSize);
// convert image at filepath to txt file
