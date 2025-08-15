
// currently only supporting PNG data format
#define STBI_ONLY_PNG
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <stdlib.h>
#include <limits.h>

#define ARGC_MIN 2
#define IMAGE_PATH_ARGV_INDEX 1
#define BLOCKSIZE_ARGV_INDEX 2

unsigned char grayscaleToChar(unsigned char grayScaleValue) {
  static const char const * Ascii = "$@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/\\|()1{}[]?-_+~<>i!lI;:,\"^`'. ";
  static const int AsciiSize = 70; // size of Ascii Array

  // flatten [0:255] values on [0:69] ids
  unsigned char idx = AsciiSize - 1 - grayScaleValue * AsciiSize / UCHAR_MAX;

  return Ascii[idx];
}

unsigned char rgbToGrayscale(unsigned char * pixel, int Ncomponents) {
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

char * createAsciiImage(unsigned char * image, int width, int height, int Ncomponents, int blockSize) {
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
          sum += rgbToGrayscale(currentPixel, Ncomponents);
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

int main(int argc, char ** argv) {
  if (argc < ARGC_MIN) {
    printf("Picture path not specified. Aborting...\n");
    return 1;
  }

  // First argument is the path of the pic which will be generated into ascii characters
  char * filename = argv[IMAGE_PATH_ARGV_INDEX];

  // Second argument is size of blocksize
  // default: 1 --> 1 pixel = 1 character
  // blocksize 3 --> 3x3 square = 1 character
  int blockSize;
  if (argc == ARGC_MIN) blockSize = 1;
  else blockSize = atoi(argv[BLOCKSIZE_ARGV_INDEX]); 
  if (blockSize < 1) {
    printf("Pre-condition: blockSize variable must be positive. Aborting...\n");
    return 1;
  }

  // Load image
  int width, height, Ncomponents;
  unsigned char * image = stbi_load(filename, &width, &height, &Ncomponents, 0);
  if (!image) {
    printf("Error loading image. Aborting...\n");
    return 1;
  }

  printf("Read image width:%d Height:%d ComponentsSize:%d\n", width, height, Ncomponents);
  char * asciiImage = createAsciiImage(image, width, height, Ncomponents, blockSize);
  if (!asciiImage) {
    return 1;
  }
  printf("%s\n", asciiImage);

  stbi_image_free(image);
  return 0;
}