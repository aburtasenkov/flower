#include "terminalInput.h"

#include "error.h"

#include <stdio.h>
#include <regex.h>
#include <string.h>

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

bool isVideo(const char * fileExtension) {
  if (strcmp(fileExtension, "mp4") == SUCCESS) return true;
  return false;
}

bool isImage(const char * fileExtension) {
  const char * ImageExtensionArray[] = {"jpg", "jpeg", "png", "bmp", "psd", "tga", "gif", "hdr", "pic", "ppm", "pgm"};
  int sz = 11;  // size of ImageExtensionArray

  for (int i = 0; i < sz; ++i) {
    if (strcmp(fileExtension, ImageExtensionArray[i]) == SUCCESS) return true;
  }
  return false;
}