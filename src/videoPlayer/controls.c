#include "controls.h"

#include "../error.h"

#include <stdio.h>
#include <unistd.h>

#define ESCAPE_CHAR '\033'
#define CONTROL_SEQUENCE_INTRODUCER '['

#define ESCAPE_SEQUENCE_ARROW_UP 'A'
#define ESCAPE_SEQUENCE_ARROW_DOWN 'B'
#define ESCAPE_SEQUENCE_ARROW_RIGHT 'C'
#define ESCAPE_SEQUENCE_ARROW_LEFT 'D'

static int get_keypress(void)
// return current key pressed
{
  unsigned char ch;
  if (read(STDIN_FILENO, &ch, 1) == 1) return ch;
  return -1;
}

static void handle_arrow_keys(UserInput * user_input, char * seq)
{
  if (seq[0] == CONTROL_SEQUENCE_INTRODUCER)
  {
    switch (seq[1])
    {
      case ESCAPE_SEQUENCE_ARROW_UP:    user_input->arrow_up = true; break;
      case ESCAPE_SEQUENCE_ARROW_DOWN:  user_input->arrow_down = true; break;
      case ESCAPE_SEQUENCE_ARROW_RIGHT: user_input->arrow_right = true; break;
      case ESCAPE_SEQUENCE_ARROW_LEFT:  user_input->arrow_left = true; break;
      default: break;
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
    case ' ':           user_input->key_space = true; break;
    case 'm': case 'M': user_input->key_m = true; break;
    case 'q': case 'Q': user_input->key_q = true; break;
    case ESCAPE_CHAR:
    {
      char seq[2];
      if (read(STDIN_FILENO, &seq[0], 1) == 1 &&
        read(STDIN_FILENO, &seq[1], 1) == 1) 
      {
        handle_arrow_keys(user_input, seq);
      }
      break;
    }
    default:
      break;
  }
}

void unpress_key(UserInput * user_input, KeyboardKey key)
// unpress KeyboardKey
{
  switch (key)
  {
    case KEY_M:       user_input->key_m = false; break;
    case KEY_Q:       user_input->key_q = false; break;
    case KEY_SPACE:   user_input->key_space = false; break;
    case ARROW_UP:    user_input->arrow_up = false; break;
    case ARROW_DOWN:  user_input->arrow_down = false; break;
    case ARROW_RIGHT: user_input->arrow_right = false; break;
    case ARROW_LEFT:  user_input->arrow_left = false; break;
    default:          raise_noncritical_error(ERROR_BAD_ARGUMENTS, "Unknown key"); break;
  }
}

bool is_pressed(UserInput * user_input, KeyboardKey key)
// return true if KeyboardKey is pressed, otherwise return false
{
  switch (key)
  {
    case KEY_M:       return user_input->key_m;
    case KEY_Q:       return user_input->key_q;
    case KEY_SPACE:   return user_input->key_space;
    case ARROW_UP:    return user_input->arrow_up;
    case ARROW_DOWN:  return user_input->arrow_down;
    case ARROW_RIGHT: return user_input->arrow_right;
    case ARROW_LEFT:  return user_input->arrow_left;
    default:
    {
      raise_noncritical_error(ERROR_BAD_ARGUMENTS, "Unknown key");
      return false;
    }
  }
}