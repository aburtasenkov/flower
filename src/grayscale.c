#include "grayscale.h"

#include <limits.h>
#include <string.h>

unsigned char grayscaleToChar(uint8_t grayscaleValue)
// map a value of 0-255 on to an array of grayscale characters and return the character
{
  static const char * Ascii = "$@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/\\|()1{}[]?-_+~<>i!lI;:,\"^`'. ";
  const size_t sz = strlen(Ascii);

  // flatten [0:255] values on [0:69] ids
  size_t index = sz - 1 - grayscaleValue * sz / UCHAR_MAX;
  if (index >= sz) index = sz - 1; // safety check

  return Ascii[index];
}