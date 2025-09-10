#include "Config.h"

#include "terminalInput.h"
#include "error.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>

#define MIN_BLOCKSIZE 1
#define MIN_FPS 0

OPTIONS loadDefaultConfig() {
  OPTIONS config = {0};
  config.FPS = 24;
  config.blockSize = 1; 
  return config;
}

void readTerminalArguments(OPTIONS * config, int argc, char ** argv) {
  if (argc < ARGC_MIN) printCriticalError(ERROR_BAD_ARGUMENTS, "Arguments not specified");

  config->filepath = argv[MEDIAPATH_ARGV_INDEX];
  for (size_t i = MEDIAPATH_ARGV_INDEX + 1; i < argc; ++i) {
    if (strcmp(argv[i], "-f") == 0 && i + 1 < argc) 
    {
      int FPS = atoi(argv[++i]);
      if (FPS < MIN_FPS) printCriticalError(ERROR_BAD_ARGUMENTS, "FPS is smaller than 0 (unlimited)");
      config->FPS = (size_t)FPS;
    } 
    else if (strcmp(argv[i], "-b") == 0 && i + 1 < argc) 
    {
      int blockSize = atoi(argv[++i]);
      if (blockSize < MIN_BLOCKSIZE) printCriticalError(ERROR_BAD_ARGUMENTS, "blockSize is smaller than 1"); 
      config->blockSize = (size_t)blockSize;
    }
    else if (strcmp(argv[i], "-o") == 0 && i + 1 < argc) 
    {
      config->output = argv[++i];
    }
    else 
    {
      printf("Warning: Unknown option '%s' ignored\n", argv[i]);
    }
  }
}