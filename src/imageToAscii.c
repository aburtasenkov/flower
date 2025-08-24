#include "imageToAscii.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "error.h"

#include <assert.h>
#include <stdbool.h>

#define NANOSECONDS_IN_SECOND 1e9

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

char * createAsciiImage(const char * filename, unsigned char * image, int width, int height, int Ncomponents, int blockSize) 
// create array of ascii characters
// this array includes \n and \o for newlines and end of string
{
  printf("Converting image %s into ascii grayscale array\n", filename);
  if (image == NULL) printCriticalError(ERROR_BAD_ARGUMENTS, "image is null pointer");
  if (width < 0 || height < 0) printCriticalError(ERROR_BAD_ARGUMENTS, "wrong image Dimensions [width: %i, height: %i]", width, height);
  if (Ncomponents < 1 || Ncomponents > 4) printCriticalError(ERROR_BAD_ARGUMENTS, "Ncomponents needs to be in range [1:4] [Ncomponents: %i]", Ncomponents);
  if (blockSize < 1) printCriticalError(ERROR_BAD_ARGUMENTS, "blockSize < 1 [blockSize: %i]", blockSize);

  int outWidth = (width + blockSize - 1) / blockSize;
  int outHeight = (height + blockSize - 1) / blockSize;

  char * asciiImage = (char *)malloc(outWidth * outHeight + outHeight + 1); // +height for "\n" and +1 for "\0" characters
  if (!asciiImage) printCriticalError(ERROR_INTERNAL, "Can not allocate enough memory for asciiImage [size in bytes: %i]", outWidth * outHeight + outHeight + 1);
  int idx = 0;

  // iterate over each pixel
  for (int by = 0; by < height; by += blockSize) {
    for (int bx = 0; bx < width; bx += blockSize) {

      long long sum = 0;
      int count = 0;

      for (int y = 0; y < blockSize && (by + y) < height; ++y) {
        for (int x = 0; x < blockSize && (bx + x) < width; ++x) {
          unsigned char * currentPixel = image + ((by + y) * width + bx + x) * Ncomponents;

          unsigned char grayscaleValue = rgbToGrayscale(currentPixel, Ncomponents);
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

void printImage(const char * filename, int blockSize) 
// convert image located at filename into ascii character array and print it out in terminal
{
  // pre-conditions
  if (filename == NULL) printCriticalError(ERROR_BAD_ARGUMENTS, "filename is null pointer");
  if (blockSize < 1) printCriticalError(ERROR_BAD_ARGUMENTS, "blockSize is smaller than 1 [blockSize: %i]", blockSize);

  // Load image
  int width, height, Ncomponents;
  unsigned char * image = stbi_load(filename, &width, &height, &Ncomponents, 0);
  if (!image) printCriticalError(ERROR_INTERNAL, "Cannot load image [filename: %s]", filename);

  printf("Read image width:%d Height:%d ComponentsSize:%d\n", width, height, Ncomponents);
  char * asciiImage = createAsciiImage(filename, image, width, height, Ncomponents, blockSize);
  if (!asciiImage) printNonCriticalError(ERROR_INTERNAL, "can not create ascii image");
  else printf("%s\n", asciiImage);

  stbi_image_free(image);
  if (asciiImage) free(asciiImage);
}

void sleepFrameTimeOffset(int FPS, timespec_t* start, timespec_t* end) 
// sleep until the next frame should be displayed
// start and end are the times of the current frame processing
{
  if (FPS < 0) printCriticalError(ERROR_BAD_ARGUMENTS, "FPS is smaller than 0 [FPS: %i]", FPS);
  if (start == NULL) printCriticalError(ERROR_BAD_ARGUMENTS, "start is null pointer");
  if (end == NULL) printCriticalError(ERROR_BAD_ARGUMENTS, "end is null pointer");
  if (start->tv_sec > end->tv_sec || (start->tv_sec == end->tv_sec && start->tv_nsec > end->tv_nsec)) printCriticalError(ERROR_BAD_ARGUMENTS, "start time is after end time");

  double frameTime = 1.0 / FPS;

  double elapsedTime = (end->tv_sec - start->tv_sec) + (end->tv_nsec - start->tv_nsec) / NANOSECONDS_IN_SECOND;
  double sleepTime = frameTime - elapsedTime;

  if (sleepTime > 0) {
    timespec_t sleepDuration;
    sleepDuration.tv_sec = (time_t)sleepTime;
    sleepDuration.tv_nsec = (long)((sleepTime - sleepDuration.tv_sec) * NANOSECONDS_IN_SECOND);
    nanosleep(&sleepDuration, NULL);
  }
}

void printImageFPS(const char * filename, int blockSize, int FPS)
// wrap printing image with FPS control
{
  if (filename == NULL) printCriticalError(ERROR_BAD_ARGUMENTS, "filename is nullpointer");
  if (blockSize < 1) printCriticalError(ERROR_BAD_ARGUMENTS, "blockSize is smaller than 1 [blockSize: %i]", blockSize);
  if (FPS < 0) printCriticalError(ERROR_BAD_ARGUMENTS, "FPS is smaller than 0 [FPS: %i]", FPS);

  timespec_t start, end;

  clock_gettime(CLOCK_MONOTONIC, &start); // start time
  printImage(filename, blockSize);
  clock_gettime(CLOCK_MONOTONIC, &end); // end time

  sleepFrameTimeOffset(FPS, &start, &end); // sleep until the next function call should be made
}