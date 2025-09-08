#include "Config.h"

#include "terminalInput.h"
#include "error.h"

#include <stdio.h>
#include <string.h>

OPTIONS loadDefaultConfig() {
  static OPTIONS config;
  config.filename = NULL;
  config.FPS = 24;
  config.blockSize = 1; 
  config.output = NULL;
  return config;
}

void readTerminalArguments(OPTIONS * config, int argc, char ** argv) {
  if (argc < ARGC_MIN) printCriticalError(ERROR_BAD_ARGUMENTS, "Arguments not specified");

  config->filename = argv[MEDIAPATH_ARGV_INDEX];
  for (int i = MEDIAPATH_ARGV_INDEX + 1; i < argc; i++) {
    if (strcmp(argv[i], "-f") == SUCCESS && i + 1 < argc) 
    {
      config->FPS = atoi(argv[++i]);
    } 
    else if (strcmp(argv[i], "-b") == SUCCESS && i + 1 < argc) 
    {
      config->blockSize = atoi(argv[++i]);
    }
    else if (strcmp(argv[i], "-o") == SUCCESS && i + 1 < argc) 
    {
      config->output = argv[++i];
    }
    else 
    {
      printf("Warning: Unknown option '%s' ignored\n", argv[i]);
    }
  }
}