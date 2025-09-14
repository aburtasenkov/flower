#include "terminalInput.h"

#include "../error.h"

#include <stdio.h>
#include <regex.h>
#include <string.h>

#define REGEX_FILE_EXTENSION_PATTERN "\\.([^.]+)$"

char * fileExtension(const char * filepath)
// return the file extension of filepath variable
{
  if (!filepath) printCriticalError(ERROR_BAD_ARGUMENTS, "filepath is null pointer");

  regex_t regex;
  regmatch_t match[2];

  int statusCode = regcomp(&regex, REGEX_FILE_EXTENSION_PATTERN, REG_EXTENDED);
  if (statusCode != 0) {
    char errorBuffer[128];
    regerror(statusCode, &regex, errorBuffer, sizeof(errorBuffer));
    printCriticalError(ERROR_RUNTIME, "Regex compilation failed: %s", errorBuffer);
  }

  statusCode = regexec(&regex, filepath, 2, match, 0);
  if (statusCode != 0) {
    char errorBuffer[128];
    regerror(statusCode, &regex, errorBuffer, sizeof(errorBuffer));
    printCriticalError(ERROR_RUNTIME, "Regex match filed: %s", errorBuffer);
  }

  int start = match[1].rm_so;
  int end = match[1].rm_eo;

  int length = end - start;
  char * extension = (char *)malloc(length + 1); // +1 for '\0'
  if (!extension) printCriticalError(ERROR_RUNTIME, "Error allocating memory for file extension");

  memcpy(extension, filepath + start, length);
  extension[length] = '\0';

  regfree(&regex);

  return extension;
}

bool isVideo(const char * fileExtension) {
  if (!fileExtension) printCriticalError(ERROR_BAD_ARGUMENTS, "fileExtension is null pointer");
  if (strcmp(fileExtension, "mp4") == 0) return true;
  return false;
}

bool isImage(const char * fileExtension) {
  static const char * ImageExtensionArray[] = {"jpg", "jpeg", "png", "bmp", "psd", "tga", "gif", "hdr", "pic", "ppm", "pgm"};
  size_t sz = 11;  // size of ImageExtensionArray

  for (size_t i = 0; i < sz; ++i) {
    if (strcmp(fileExtension, ImageExtensionArray[i]) == 0) return true;
  }

  return false;
}