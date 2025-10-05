#include "controls.h"

#include <stdio.h>
#include <unistd.h>

#define ESC_SEQ_LENGTH 3
#define ESC_SEQ_FIRST 0
#define ESC_SEQ_SECOND 1

#define ARROW_LEFT 'D'
#define ARROW_RIGHT 'C'

static int get_keypress(void)
// return current key pressed
{
  unsigned char ch;
  if (read(STDIN_FILENO, &ch, 1) == 1) return ch;
  return -1;
}

static void handle_arrow_keys(UserInput * user_input, char * seq)
{
  if (seq[ESC_SEQ_FIRST] == '[')
  {
    switch (seq[ESC_SEQ_SECOND])
    {
      case ARROW_LEFT:
        user_input->arrow_left = true;
        break;
      case ARROW_RIGHT:
        user_input->arrow_right = true;
        break;
      default:
        break;
    }
  }
}

void check_keypress(UserInput * user_input)
// check current key pressed and set the corresponding member of user_input to true
{
  int key = get_keypress();
  if (key == -1) return; 

  switch (key)
  {
    case ' ':
      user_input->space = true;
      break;
    
    case 'q':
      user_input->lowercase_q = true;
      break;
    
    case '\033': // escape sequence
    {
      char seq[ESC_SEQ_LENGTH];
      if (read(STDIN_FILENO, &seq[ESC_SEQ_FIRST], 1) == 1 &&
        read(STDIN_FILENO, &seq[ESC_SEQ_SECOND], 1) == 1) 
      {
        handle_arrow_keys(user_input, seq);
      }
      break;
    }
    default:
      break;
  }
}
