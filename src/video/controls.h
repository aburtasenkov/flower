#pragma once

#include <stdbool.h>

extern bool ESCAPE_LOOP;
extern bool PAUSE;

int get_keypress();
// return current key pressed

void check_keypress();
// set flags on key presses