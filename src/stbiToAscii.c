#include "stbiToAscii.h"

#include "error.h"

#include <assert.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>

static inline uint8_t rgbToGrayscale(const unsigned char * pixel, size_t Ncomponents) 
// convert single or multichannel pixel to grayscale value [0:255]
{
  static const float RGB_FACTORS[] = {0.299f, 0.587f, 0.114f};
  
  if (Ncomponents == 1 || Ncomponents == 2) return pixel[0];

  float gray = 0.0f;
  for (size_t i = 0; i < 3; ++i) {
    gray += pixel[i] * RGB_FACTORS[i];
  }
  return (uint8_t)gray;
}

static inline unsigned char grayscaleToChar(uint8_t grayscaleValue)
// map a value of 0-255 on to an array of grayscale characters and return the character
{
  static const char Ascii[] = "$@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/\\|()1{}[]?-_+~<>i!lI;:,\"^`'. ";
  static const size_t sz = sizeof(Ascii) - 1;

  // flatten [0:255] values on [0:sz[ ids
  size_t index = (size_t)((float)grayscaleValue / 255.0f * (sz - 1));

  return Ascii[index];
}

char * stbiToAscii(const ImageStbi * stbi, size_t blockSize) 
// create array of ascii characters
// this array includes \n and \o for newlines and end of string
// no checks and therefore the caller must ensure stbi is valid pointer
{
  size_t outWidth = (stbi->width + blockSize - 1) / blockSize;
  size_t outHeight = (stbi->height + blockSize - 1) / blockSize;

  char * asciiImage = (char *)malloc(outWidth * outHeight + outHeight + 1); // +height for "\n" and +1 for "\0" characters
  if (!asciiImage) printCriticalError(ERROR_RUNTIME, "Can not allocate enough memory for asciiImage [size in bytes: %zu]", outWidth * outHeight + outHeight + 1);
  size_t idx = 0;

  // iterate over each pixel
  for (size_t by = 0; by < stbi->height; by += blockSize) {
    for (size_t bx = 0; bx < stbi->width; bx += blockSize) {

      size_t sum = 0;
      size_t count = 0;

      for (int y = 0; y < blockSize && (by + y) < stbi->height; ++y) {
        for (int x = 0; x < blockSize && (bx + x) < stbi->width; ++x) {
          unsigned char * currentPixel = stbi->data + ((by + y) * stbi->width + bx + x) * stbi->Ncomponents;

          unsigned char grayscaleValue = rgbToGrayscale(currentPixel, stbi->Ncomponents);
          sum += grayscaleValue;
          ++count;
        }
      }

      unsigned char averageGray = sum / count;
      asciiImage[idx++] = grayscaleToChar(averageGray);
    }
    if (by + blockSize < stbi->height)
      asciiImage[idx++] = '\n';
  }
  asciiImage[idx] = '\0';
  return asciiImage;
}

void printImage(const char * filepath, size_t blockSize) 
// convert image located at filepath into ascii character array and print it out in terminal
{
  if (!filepath) printCriticalError(ERROR_BAD_ARGUMENTS, "filepath is null pointer");

  ImageStbi * stbi = loadStbi(filepath);
  printf("Read image width:%zu Height:%zu ComponentsSize:%zu\n", stbi->width, stbi->height, stbi->Ncomponents);

  printf("Converting image %s into ascii grayscale array\n", filepath);
  char * asciiImage = stbiToAscii(stbi, blockSize);
  freeStbi(stbi);

  printf("%s\n", asciiImage);
  free(asciiImage);
}
