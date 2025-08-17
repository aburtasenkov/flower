#include "imageToAscii.h"
#include "videoToAscii.h"

#include <stdlib.h>
#include <stdio.h>
#include <regex.h>
#include <sys/wait.h>

#define ARGC_MIN 2
#define IMAGE_PATH_ARGV_INDEX 1
#define BLOCKSIZE_ARGV_INDEX 2

#define REGEX_FILE_EXTENSION_PATTERN "\\.([^.]+)$"

char * filenameExtension(const char * filename)
// return the file extension of filename variable
{
  if (filename == NULL) {
    printf("Pre-condition fileNameExtension(const char * filename): filename is null pointer\n");
    return NULL;
  }

  regex_t regex;
  regmatch_t match[2];

  int statusCode = regcomp(&regex, REGEX_FILE_EXTENSION_PATTERN, REG_EXTENDED);
  if (statusCode != 0) {
    char errorBuffer[128];
    regerror(statusCode, &regex, errorBuffer, sizeof(errorBuffer));
    printf("Regex compilation failed: %s... Aborting\n", errorBuffer);
    return NULL;
  }

  statusCode = regexec(&regex, filename, 2, match, 0);
  if (statusCode != 0) {
    char errorBuffer[128];
    regerror(statusCode, &regex, errorBuffer, sizeof(errorBuffer));
    printf("Regex match failed: %s... Aborting\n", errorBuffer);
    return NULL;
  }

  int start = match[1].rm_so;
  int end = match[1].rm_eo;

  int length = end - start;
  char * extension = (char *)malloc(length + 1); // +1 for '\0'

  if (!extension) {
    printf("Error allocating memory for file extension. Aborting...\n");
    return NULL;
  }

  memcpy(extension, filename + start, length);
  extension[length] = '\0';

  regfree(&regex);

  return extension;
}

int main(int argc, char ** argv) {
  /*-----------------------Load terminal input parameters-----------------------*/
  if (argc < ARGC_MIN) {
    printf("Picture path not specified. Aborting...\n");
    return 1;
  }

  // First argument is the path of the pic which will be generated into ascii characters
  char * filename = argv[IMAGE_PATH_ARGV_INDEX];

  char * fileExtension = filenameExtension(filename);
  if (!fileExtension) return 1;

  printf("File Extension: %s\n", filenameExtension(filename));

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


  if (strcmp(fileExtension, "png") == 0) printImage(filename, blockSize);
  if (strcmp(fileExtension, "mp4") == 0) printVideo(filename, blockSize);

  free(fileExtension);

  return 0;
}