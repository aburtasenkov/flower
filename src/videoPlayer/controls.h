#pragma once

#include <stdbool.h>

typedef struct {
  bool lowercase_q;
  bool space;
  bool arrow_left;
  bool arrow_right;
  bool arrow_up;
  bool arrow_down;
} UserInput;

void check_keypress(UserInput *);
// set UserInput's members on keypresses to true