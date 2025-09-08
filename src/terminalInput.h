#pragma once

#include <stdbool.h>

#define ARGC_MIN 2
#define MEDIAPATH_ARGV_INDEX 1

#define REGEX_FILE_EXTENSION_PATTERN "\\.([^.]+)$"

char * fileExtension(const char * filepath);
// return the file extension of filepath variable

bool isVideo(const char * fileExtension);

bool isImage(const char * fileExtension);