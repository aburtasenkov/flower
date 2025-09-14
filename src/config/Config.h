#pragma once

#include <stddef.h>

typedef struct {
  char * filepath;
  size_t FPS;
  size_t blockSize;
  char * output;
} OPTIONS;

OPTIONS loadDefaultConfig();

void readTerminalArguments(OPTIONS * config, int argc, char ** argv);