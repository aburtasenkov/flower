#pragma once

#include <stddef.h>
#include <stdbool.h>

bool file_exists(const char *);

char * file_extension(const char *);
// return the file extension of filepath variable

void write_image(const char *, const char *, const size_t);
// convert image at filepath to ascii and write the resulting image in output_path
// if file format is not supported, function will cause an error
