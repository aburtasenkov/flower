#include "imageToAscii.h"

#include <stdlib.h>

#define ARGC_MIN 2
#define IMAGE_PATH_ARGV_INDEX 1
#define BLOCKSIZE_ARGV_INDEX 2

int main(int argc, char ** argv) {
  /*-----------------------Load terminal input parameters-----------------------*/
  if (argc < ARGC_MIN) {
    printf("Picture path not specified. Aborting...\n");
    return 1;
  }

  // First argument is the path of the pic which will be generated into ascii characters
  char * filename = argv[IMAGE_PATH_ARGV_INDEX];
  
  // Load image
  int width, height, Ncomponents;
  unsigned char * image = stbi_load(filename, &width, &height, &Ncomponents, 0);
  if (!image) {
    printf("Error loading image. Aborting...\n");
    return 1;
  }

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

  /*-----------------------Loading done-----------------------*/

  printf("Read image width:%d Height:%d ComponentsSize:%d\n", width, height, Ncomponents);
  char * asciiImage = createAsciiImage(image, width, height, Ncomponents, blockSize);
  if (!asciiImage) {
    return 1;
  }
  printf("%s\n", asciiImage);

  stbi_image_free(image);
  return 0;
}