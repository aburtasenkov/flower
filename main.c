#include "src/imageToAscii.h"
#include "src/videoToAscii.h"

#include "src/error.h"

#include <stdlib.h>
#include <stdio.h>
#include <regex.h>
#include <string.h>

#define ARGC_MIN 2
#define IMAGE_PATH_ARGV_INDEX 1

#define REGEX_FILE_EXTENSION_PATTERN "\\.([^.]+)$"

typedef struct {
  char * filename;
  int FPS;
  int blockSize;
} OPTIONS;

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
  if (statusCode != SUCCESS) {
    char errorBuffer[128];
    regerror(statusCode, &regex, errorBuffer, sizeof(errorBuffer));
    printf("Regex compilation failed: %s... Aborting\n", errorBuffer);
    return NULL;
  }

  statusCode = regexec(&regex, filename, 2, match, 0);
  if (statusCode != SUCCESS) {
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

OPTIONS loadDefaultConfig() {
  static OPTIONS config;
  config.filename = NULL;
  config.FPS = 24;
  config.blockSize = 1; 
  return config;
}

bool isVideo(const char * fileExtension) {
  if (strcmp(fileExtension, "mp4") == SUCCESS) return true;
  return false;
}

bool isImage(const char * fileExtension) {
  const char * ImageExtensionArray[] = {"jpg", "jpeg", "png", "bmp", "psd", "tga", "gif", "hdr", "pic", "ppm", "pgm"};
  int count = 11;

  for (int i = 0; i < 11; ++i) {
    if (strcmp(fileExtension, ImageExtensionArray[i]) == SUCCESS) return true;
  }
  return false;
}

void readTerminalArguments(OPTIONS * config, int argc, char ** argv) {
  if (argc < ARGC_MIN) printCriticalError(ERROR_BAD_ARGUMENTS, "Arguments not specified");

  config->filename = argv[IMAGE_PATH_ARGV_INDEX];
  for (int i = IMAGE_PATH_ARGV_INDEX + 1; i < argc; i++) {
      if (strcmp(argv[i], "-f") == SUCCESS && i + 1 < argc) {
          config->FPS = atoi(argv[++i]);
      } else if (strcmp(argv[i], "-b") == SUCCESS && i + 1 < argc) {
          config->blockSize = atoi(argv[++i]);
      } else {
          printf("Warning: Unknown option '%s' ignored\n", argv[i]);
      }
  }
}

int main(int argc, char ** argv) {
  /*-----------------------Load parameters from terminal-----------------------*/
  OPTIONS config = loadDefaultConfig();
  readTerminalArguments(&config, argc, argv);

  char * fileExtension = filenameExtension(config.filename);
  if (!fileExtension) printCriticalError(ERROR_BAD_ARGUMENTS, "No file extension found in %s", config.filename);
  else printf("File Extension: %s\n", fileExtension);

  /*-----------------------Execute code-----------------------*/

  if (isVideo(fileExtension)) printVideo(config.filename, config.blockSize, config.FPS);
  if (isImage(fileExtension)) printImage(config.filename, config.blockSize);

  free(fileExtension);

  return SUCCESS;
}