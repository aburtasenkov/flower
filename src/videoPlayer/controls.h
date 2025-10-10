#pragma once

#include <stdbool.h>

typedef enum {
  KEY_M, KEY_Q, KEY_SPACE, ARROW_UP, ARROW_DOWN, ARROW_RIGHT, ARROW_LEFT
} KeyboardKey;

typedef struct {
  bool key_m;
  bool key_q;
  bool key_space;
  bool arrow_up;
  bool arrow_down;
  bool arrow_right;
  bool arrow_left;
} UserInput;

void check_keypress(UserInput *);
// set UserInput's members on keypresses to true

void unpress_key(UserInput *, KeyboardKey);
// unpress KeyboardKey

bool is_pressed(UserInput *, KeyboardKey);
// return true if KeyboardKey is pressed, otherwise return false