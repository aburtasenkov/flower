#pragma once

#include <stdbool.h>

typedef struct {
  bool escape;
  bool space;
  bool arrow_left;
  bool arrow_right;
} UserInput;

void check_keypress(UserInput *);
// set UserInput's members on keypresses to true