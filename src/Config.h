#pragma once

typedef struct {
  char * filepath;
  int FPS;
  int blockSize;
  char * output;
} OPTIONS;

OPTIONS loadDefaultConfig();

void readTerminalArguments(OPTIONS * config, int argc, char ** argv);