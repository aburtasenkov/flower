
#include <assert.h>

#define NANOSECONDS_IN_SECOND 1e9

unsigned char rgbToGrayscale(unsigned char * pixel, int Ncomponents) 
// convert rgb value of a pixel to an ascii char
{
  // pixel not null pointer
  assert(pixel != NULL);
  // Ncomponents in range[1:4]
  assert(Ncomponents > 0 && Ncomponents < 5);

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
// create array of ascii chars for output
// returns NULL at error
{
  printf("Converting image %s into ascii grayscale array\n", filename);
  if (image == NULL) {
    printf("Pre-condition createAsciiImage(const char * filename, unsigned char * image, int width, int height, int Ncomponents, int blockSize): image is null pointer\n");
    return NULL;
  }
  if (width < 0 || height < 0) {
    printf("Pre-condition createAsciiImage(const char * filename, unsigned char * image, int width, int height, int Ncomponents, int blockSize): wrong image Dimensions (width or height are smaller than 1 pixel)\n");
    return NULL;
  }
  if (Ncomponents < 1 || Ncomponents > 4) {
    printf("Pre-condition createAsciiImage(const char * filename, unsigned char * image, int width, int height, int Ncomponents, int blockSize): Ncomponents needs to be in range [1:4]\n");
    return NULL;
  }
  if (blockSize < 1) {
    printf("Pre-condition createAsciiImage(const char * filename, unsigned char * image, int width, int height, int Ncomponents, int blockSize): blockSize is smaller than 1");
    return NULL;
  }

  int outWidth = (width + blockSize - 1) / blockSize;
  int outHeight = (height + blockSize - 1) / blockSize;

  char * asciiImage = (char *)malloc(outWidth * outHeight + outHeight + 1); // +height for "\n" and +1 for "\0" characters
  if (!asciiImage) {
    printf("Cannot malloc enough space for asciiImage variable. Aborting...\n");
    return NULL;
  }
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
  if (filename == NULL) {
    printf("Pre-condition printImage(const char * filename, int blockSize): filename is null pointer\n");
    return;
  }
  if (blockSize < 1) {
    printf("Pre-condition printImage(const char * filename, int blockSize): blockSize is smaller than 1");
    return;
  }

  // Load image
  int width, height, Ncomponents;
  unsigned char * image = stbi_load(filename, &width, &height, &Ncomponents, 0);
  if (!image) {
    printf("Error loading image. Aborting...\n");
    return;
  }

  printf("Read image width:%d Height:%d ComponentsSize:%d\n", width, height, Ncomponents);
  char * asciiImage = createAsciiImage(filename, image, width, height, Ncomponents, blockSize);
  if (!asciiImage) {
    printf("Error converting image to ascii. Aborting...\n");
    return;
  }
  else printf("%s\n", asciiImage);

  stbi_image_free(image);
  if (asciiImage) free(asciiImage);
}

void sleepFrameTimeOffset(int FPS, timespec_t* start, timespec_t* end) 
// sleep until the next frame should be displayed
// start and end are the times of the current frame processing
{
  if (FPS <= 0) {
    printf("Pre-condition sleepFrameTimeOffset(int FPS, timespec_t * start, timespec_t * end): FPS is smaller than or equal to 0\n");
    return;
  }
  if (start == NULL || end == NULL) {
    printf("Pre-condition sleepFrameTimeOffset(int FPS, timespec_t * start, timespec_t * end): start or end is null pointer\n");
    return;
  }
  if (start->tv_sec > end->tv_sec || (start->tv_sec == end->tv_sec && start->tv_nsec > end->tv_nsec)) {
    printf("Pre-condition sleepFrameTimeOffset(int FPS, timespec_t * start, timespec_t * end): start time is after end time\n");
    return;
  }

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

void printImageFPS(int FPS, const char * filename, int blockSize)
// wrapper print Image with FPS control using FPSWrapper function
{
  if (FPS < 0) {
    printf("Pre-condition printImageFPS(int FPS, const char * filename, int blockSize): FPS is smaller than 0\n");
    return;
  }
  if (filename == NULL) {
    printf("Pre-condition printImageFPS(int FPS, const char * filename, int blockSize): function or filename is null pointer\n");
    return;
  }
  if (blockSize < 1) {
    printf("Pre-condition printImageFPS(int FPS, const char * filename, int blockSize): blockSize is smaller than 1\n");
    return;
  }

  timespec_t start, end;

  clock_gettime(CLOCK_MONOTONIC, &start); // start time
  printImage(filename, blockSize);
  clock_gettime(CLOCK_MONOTONIC, &end); // end time

  sleepFrameTimeOffset(FPS, &start, &end); // sleep until the next function call should be made
}