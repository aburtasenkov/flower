#include <limits.h>

unsigned char grayscaleToChar(unsigned char grayScaleValue)
// map a value of 0-255 on to an array of grayscale characters and return the character
{
  static const char const * Ascii = "$@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/\\|()1{}[]?-_+~<>i!lI;:,\"^`'. ";
  static const int AsciiSize = 70; // size of Ascii Array

  // flatten [0:255] values on [0:69] ids
  unsigned char idx = AsciiSize - 1 - grayScaleValue * AsciiSize / UCHAR_MAX;

  return Ascii[idx];
}