
// currently only supporting PNG data format
#define STBI_ONLY_PNG
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <stdlib.h>
#include <limits.h>

#define ARGC_MIN 2
#define IMAGE_PATH_ARGV_INDEX 1

unsigned char grayscaleToChar(unsigned char grayScaleValue) {
  static const char const * Ascii = "$@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/\\|()1{}[]?-_+~<>i!lI;:,\"^`'. ";
  static const int AsciiSize = 70;

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

char * createAsciiImage(unsigned char * image, int width, int height, int Ncomponents) {
  char * asciiImage = (char *)malloc(width * height + height + 1); // +height for "\n" and +1 for "\0" characters
  if (!asciiImage) {
    printf("Cannot malloc enough space for asciiImage variable. Aborting...\n");
    stbi_image_free(image);
    return 1;
  }

  int idx = 0;
  // iterate over each pixel
  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      unsigned char * currentPixel = image + (y * width + x) * Ncomponents;

      asciiImage[idx++] = grayscaleToChar(rgbToGrayscale(currentPixel, Ncomponents));
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

  char * filename = argv[IMAGE_PATH_ARGV_INDEX];

  // Load image
  int width, height, Ncomponents;
  unsigned char * image = stbi_load(filename, &width, &height, &Ncomponents, 0);
  if (!image) {
    printf("Error loading image. Aborting...\n");
    return 1;
  }

  printf("Read image width:%d Height:%d ComponentsSize:%d\n", width, height, Ncomponents);
  printf("%s\n", createAsciiImage(image, width, height, Ncomponents));

  stbi_image_free(image);
  return 0;
}