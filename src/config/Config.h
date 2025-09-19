#pragma once

#include <stddef.h>

typedef struct {
  char * filepath;
  size_t block_sz;
  char * output;
} OPTIONS;

OPTIONS default_config();

void parse_options(OPTIONS * config, int argc, char ** argv);