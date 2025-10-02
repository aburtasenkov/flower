#pragma once

#include <stdbool.h>

#define ARGC_MIN 2
#define MEDIAPATH_ARGV_INDEX 1

// OS dependent command to clear the terminal
#ifdef _WIN32
#define CLEAR_COMMAND  "cls"
#else
#define CLEAR_COMMAND "clear"
#endif

int execute_command(const char *);
// Wrapper for system(*command*) calls in order to ease debugging

bool is_video(const char *);

bool is_image(const char *);

void disable_raw_mode(void);
// reset stdin flags

void enable_raw_mode(void);
// make stdin non blocking, disable canonical and echo modes