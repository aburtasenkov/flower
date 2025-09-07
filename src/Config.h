#pragma once

typedef struct {
  char * filename;
  int FPS;
  int blockSize;
} OPTIONS;

OPTIONS loadDefaultConfig();

void readTerminalArguments(OPTIONS * config, int argc, char ** argv);