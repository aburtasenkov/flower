#pragma once

extern const char const * supportedOutputFormats[];

void writeImage(const char * filepath, const char * outputPath, int blockSize);
// convert image at filepath to ascii and write the resulting image in outputPath
// if file format is not supported, function will cause an error
