#include "controls.h"

#include <stdio.h>
#include <unistd.h>

bool ESCAPE_LOOP = false;
bool PAUSE = false;
bool MOVE_LEFT = false;
bool MOVE_RIGHT = false;

int get_keypress()
// return current key pressed
{
  unsigned char ch;
  if (read(STDIN_FILENO, &ch, 1) == 1) return ch;
  return -1;
}

void check_keypress()
// set flags on key presses git commit -m "Moved video player controls to their own header file"
{
  int key = get_keypress();
  if (key == -1) return; 

  if (key == ' ') // space to toggle pause
  {
    PAUSE = !PAUSE;
  }
  else if (key == 'q') // quit
  {
    ESCAPE_LOOP = !ESCAPE_LOOP;
  } 
  else if (key == '\033') // arrow keys start with escape sequence
  {
    char seq[3];
    if (read(STDIN_FILENO, &seq[0], 1) == 1 &&
      read(STDIN_FILENO, &seq[1], 1) == 1) 
      {
      if (seq[0] == '[') 
      {
        if (seq[1] == 'D') 
        {
          // left arrow
          MOVE_LEFT = true;
        } 
        else if (seq[1] == 'C') 
        {
          // right arrow
          MOVE_RIGHT = true;
        }
      }
    }
  }
}
