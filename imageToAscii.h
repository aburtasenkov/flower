#pragma once 

#include "grayscale.h"

// Allow only certain data files

// #define STBI_NO_JPEG
// #define STBI_NO_PNG
#define STBI_NO_BMP
#define STBI_NO_PSD
#define STBI_NO_TGA
#define STBI_NO_GIF
#define STBI_NO_HDR
#define STBI_NO_PIC
#define STBI_NO_PNM   // (.ppm and .pgm)

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

unsigned char rgbToGrayscale(unsigned char * pixel, int Ncomponents) 
// convert rgb value of a pixel to an ascii char
// return ERROR_GRAYSCALE_VAL at error
{
  // pre conditions
  if (pixel == NULL) {
    printf("Pre-condition rgbToGrayscale(unsigned char * pixel, int Ncomponents): pixel is null pointer\n");
    return ERROR_GRAYSCALE_VAL;
  }
  if (Ncomponents < 1 || Ncomponents > 4) {
    printf("Pre-condition rgbToGrayscale(unsigned char * pixel, int Ncomponents): Ncomponents needs to be in range [1:4]\n");
    return ERROR_GRAYSCALE_VAL;
  }

  static const float RGB_FACTORS[] = {0.299f, 0.587f, 0.114f};

  unsigned char grayscale = 0;
  
  // if 3 or more components -> calculate grayscale value
  // otherwise grayscale value is present in current pixel (first component of the pixel)
  switch (Ncomponents) {
    case 1: case 2:
      grayscale = *pixel;
      break;
    case 3: case 4:
      for (int i = 0; i < 3; ++i) {
        grayscale += pixel[i] * RGB_FACTORS[i];
      }
      break;
  }
  return grayscale;
}

char * createAsciiImage(unsigned char * image, int width, int height, int Ncomponents, int blockSize) 
// create array of ascii chars for output
// returns NULL at error
{
  int outWidth = width / blockSize;
  int outHeight = height / blockSize;

  char * asciiImage = (char *)malloc(outWidth * outHeight + outHeight + 1); // +height for "\n" and +1 for "\0" characters
  if (!asciiImage) {
    printf("Cannot malloc enough space for asciiImage variable. Aborting...\n");
    stbi_image_free(image);
    return NULL;
  }
  int idx = 0;

  // iterate over each pixel
  for (int by = 0; by < height; by += blockSize) {
    for (int bx = 0; bx < width; bx += blockSize) {

      int sum = 0;
      int count = 0;

      for (int y = 0; y < blockSize && (by + y) < height; ++y) {
        for (int x = 0; x < blockSize && (bx + x) < width; ++x) {
          unsigned char * currentPixel = image + ((by + y) * width + bx + x) * Ncomponents;

          unsigned char grayscaleValue = rgbToGrayscale(currentPixel, Ncomponents);
          if (grayscaleValue == ERROR_GRAYSCALE_VAL) {
            free(asciiImage);
            return NULL;
          }
          sum += grayscaleValue;
          ++count;
        }
      }

      unsigned char averageGray = sum / count;
      asciiImage[idx++] = grayscaleToChar(averageGray);
    }
    asciiImage[idx++] = '\n';
  }
  asciiImage[idx] = '\0';
  return asciiImage;
}
