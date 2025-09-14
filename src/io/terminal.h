#pragma once

#include <stdbool.h>

#define ARGC_MIN 2
#define MEDIAPATH_ARGV_INDEX 1

// OS dependent command to clear the terminal
#ifdef _WIN32
#define ClearCommand  "cls"
#else
#define ClearCommand "clear"
#endif

int execute_command(const char * command);
// Wrapper for system(*command*) calls in order to ease debugging

bool is_video(const char * file_extension);

bool is_image(const char * file_extension);