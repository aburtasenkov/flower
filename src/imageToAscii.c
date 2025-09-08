#include "imageToAscii.h"

#include "error.h"
#include "grayscale.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <assert.h>
#include <stdbool.h>

ImageStbi * loadStbi(const char * filepath)
// create object of ImageStbi class that loads images in format that is supported by stbi
{
  if (!filepath) printCriticalError(ERROR_BAD_ARGUMENTS, "filepath is null pointer");

  ImageStbi * stbi = (ImageStbi *)malloc(sizeof(ImageStbi));
  if (!stbi) printCriticalError(ERROR_RUNTIME, "Cannot allocate enough memory for stbi Image [size in bytes: %zu]", sizeof(ImageStbi));

  int width, height, Ncomponents;
  unsigned char * data = stbi_load(filepath, &width, &height, &Ncomponents, 0);
  if (!data) printCriticalError(ERROR_INTERNAL, "Cannot load image [filepath: %s]", filepath);

  stbi->data = data;
  stbi->width = width;
  stbi->height = height;
  stbi->Ncomponents = Ncomponents;

  return stbi;
}

void freeStbi(ImageStbi * stbi) {
  if (!stbi) return;
  if (!stbi->data) { free(stbi); return; }

  stbi_image_free(stbi->data);
  free(stbi);
}

unsigned char rgbToGrayscale(unsigned char * pixel, int Ncomponents) 
// convert single or multichannel pixel to grayscale value [0:255]
{
  if (pixel == NULL) printCriticalError(ERROR_BAD_ARGUMENTS, "pixel is null pointer");
  if (!(Ncomponents > 0 && Ncomponents < 5)) printCriticalError(ERROR_BAD_ARGUMENTS, "Ncomponents is not in range [1:4] [Ncomponents: %i]", Ncomponents);

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

char * stbiToAscii(ImageStbi * stbi, int blockSize) 
// create array of ascii characters
// this array includes \n and \o for newlines and end of string
{
  if (!stbi) printCriticalError(ERROR_BAD_ARGUMENTS, "stbi is null pointer");
  if (!stbi->data) printCriticalError(ERROR_BAD_ARGUMENTS, "stbi->data is null pointer");
  if (stbi->width < 0 || stbi->height < 0) printCriticalError(ERROR_BAD_ARGUMENTS, "wrong image Dimensions [width: %i, height: %i]", stbi->width, stbi->height);
  if (stbi->Ncomponents < 1 || stbi->Ncomponents > 4) printCriticalError(ERROR_BAD_ARGUMENTS, "Ncomponents needs to be in range [1:4] [Ncomponents: %i]", stbi->Ncomponents);
  if (blockSize < 1) printCriticalError(ERROR_BAD_ARGUMENTS, "blockSize < 1 [blockSize: %i]", blockSize);

  int outWidth = (stbi->width + blockSize - 1) / blockSize;
  int outHeight = (stbi->height + blockSize - 1) / blockSize;

  char * asciiImage = (char *)malloc(outWidth * outHeight + outHeight + 1); // +height for "\n" and +1 for "\0" characters
  if (!asciiImage) printCriticalError(ERROR_INTERNAL, "Can not allocate enough memory for asciiImage [size in bytes: %i]", outWidth * outHeight + outHeight + 1);
  int idx = 0;

  // iterate over each pixel
  for (int by = 0; by < stbi->height; by += blockSize) {
    for (int bx = 0; bx < stbi->width; bx += blockSize) {

      long long sum = 0;
      int count = 0;

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
    asciiImage[idx++] = '\n';
  }
  asciiImage[idx] = '\0';
  return asciiImage;
}

void printImage(const char * filepath, int blockSize) 
// convert image located at filepath into ascii character array and print it out in terminal
{
  // pre-conditions
  if (filepath == NULL) printCriticalError(ERROR_BAD_ARGUMENTS, "filepath is null pointer");
  if (blockSize < 1) printCriticalError(ERROR_BAD_ARGUMENTS, "blockSize is smaller than 1 [blockSize: %i]", blockSize);

  // Load image
  ImageStbi * stbi = loadStbi(filepath);
  printf("Read image width:%d Height:%d ComponentsSize:%d\n", stbi->width, stbi->height, stbi->Ncomponents);

  printf("Converting image %s into ascii grayscale array\n", filepath);
  char * asciiImage = stbiToAscii(stbi, blockSize);
  if (!asciiImage) printNonCriticalError(ERROR_INTERNAL, "can not create ascii image");
  else printf("%s\n", asciiImage);

  freeStbi(stbi);
  if (asciiImage) free(asciiImage);
}
