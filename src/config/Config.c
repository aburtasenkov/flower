#include "Config.h"

#include "../io/terminalInput.h"
#include "../error.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>

#define MIN_BLOCKSIZE 1
#define MIN_FPS 0

#define OPTION_FPS "-f"
#define OPTION_BLOCKSIZE "-b"
#define OPTION_OUTPUT "-o"

OPTIONS default_config() {
  OPTIONS config = {0};
  config.FPS = 24;
  config.block_sz = 1; 
  return config;
}

void parse_options(OPTIONS * config, int argc, char ** argv) {
  if (argc < ARGC_MIN) printCriticalError(ERROR_BAD_ARGUMENTS, "Arguments not specified");

  config->filepath = argv[MEDIAPATH_ARGV_INDEX];
  for (size_t i = MEDIAPATH_ARGV_INDEX + 1; (int)i < argc; ++i) {
    if (strcmp(argv[i], OPTION_FPS) == 0 && (int)(i + 1) < argc) 
    {
      int FPS = atoi(argv[++i]);
      if (FPS < MIN_FPS) printCriticalError(ERROR_BAD_ARGUMENTS, "FPS is smaller than 0 (unlimited)");
      config->FPS = (size_t)FPS;
    } 
    else if (strcmp(argv[i], OPTION_BLOCKSIZE) == 0 && (int)(i + 1) < argc) 
    {
      int block_sz = atoi(argv[++i]);
      if (block_sz < MIN_BLOCKSIZE) printCriticalError(ERROR_BAD_ARGUMENTS, "block_sz is smaller than 1"); 
      config->block_sz = (size_t)block_sz;
    }
    else if (strcmp(argv[i], OPTION_OUTPUT) == 0 && (int)(i + 1) < argc) 
    {
      config->output = argv[++i];
    }
    else 
    {
      printf("Warning: Unknown option '%s' ignored\n", argv[i]);
    }
  }
}