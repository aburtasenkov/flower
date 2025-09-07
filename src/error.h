#pragma once

#include <stdio.h>
#include <stdlib.h>

#define SUCCESS EXIT_SUCCESS
#define ERROR_BAD_ARGUMENTS 1
#define ERROR_INTERNAL 2
#define ERROR_EXTERNAL 3
#define ERROR_RUNTIME 4

#define printCriticalError(ERROR_CODE, fmt, ...) \
    do { \
        fprintf(stderr, "Error with code %d: " fmt "\n\tat %s:%d in function %s\n", \
                (int)ERROR_CODE, ##__VA_ARGS__, __FILE__, __LINE__, __func__); \
        exit(ERROR_CODE); \
    } while (0)

#define printNonCriticalError(ERROR_CODE, fmt, ...) \
        fprintf(stderr, "Error with code %d: " fmt "\n\tat %s:%d in function %s\n", \
                (int)ERROR_CODE, ##__VA_ARGS__, __FILE__, __LINE__, __func__)

