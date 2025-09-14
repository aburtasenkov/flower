#pragma once

#include <stdbool.h>

#define ARGC_MIN 2
#define MEDIAPATH_ARGV_INDEX 1

char * file_extension(const char * filepath);
// return the file extension of filepath variable

bool is_video(const char * file_extension);

bool is_image(const char * file_extension);