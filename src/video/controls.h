#pragma once

#include <stdbool.h>

extern bool ESCAPE_LOOP;
extern bool PAUSE;
extern bool MOVE_LEFT;
extern bool MOVE_RIGHT;

int get_keypress();
// return current key pressed

void check_keypress();
// set flags on key presses