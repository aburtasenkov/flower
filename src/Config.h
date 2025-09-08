#pragma once

typedef struct {
  char * filename;
  int FPS;
  int blockSize;
  char * output;
} OPTIONS;

OPTIONS loadDefaultConfig();

void readTerminalArguments(OPTIONS * config, int argc, char ** argv);