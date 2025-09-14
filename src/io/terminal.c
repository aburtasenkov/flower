#include "terminal.h"

#include "../error.h"

#include <stdio.h>
#include <regex.h>
#include <string.h>

#define REGEX_FILE_EXTENSION_PATTERN "\\.([^.]+)$"

int execute_command(const char * command) 
// Wrapper for system(*command*) calls in order to ease debugging
{
  int status_code = system(command);

  // check if system failed
  if (status_code == -1) raise_critical_error(ERROR_RUNTIME, "execute_command(\"%s\") failed", command);

  // it exited normally --> check exit code
  if (WIFEXITED(status_code)) {
    int exit_code = WEXITSTATUS(status_code);

    if (exit_code == EXIT_SUCCESS) {
      return EXIT_SUCCESS;
    }

    raise_noncritical_error(exit_code, "\"%s\" ran unsucessfully (exit code %d).\n", command, exit_code);
    return exit_code;
  }
  // check for signal termination
  if (WIFSIGNALED(status_code)) {
    int sig = WTERMSIG(status_code);
    sig += 128; // common convention for signal termination
    raise_noncritical_error(sig, "\"%s\" was terminated by signal %d.\n", command, sig);
    return sig;
  }
  // all other non formal terminations
  raise_critical_error(ERROR_RUNTIME, "system(\"%s\") did not terminate normally", command);
  return 1;
}

char * file_extension(const char * filepath)
// return the file extension of filepath variable
{
  if (!filepath) raise_critical_error(ERROR_BAD_ARGUMENTS, "filepath is null pointer");

  regex_t regex;
  regmatch_t match[2];

  int status_code = regcomp(&regex, REGEX_FILE_EXTENSION_PATTERN, REG_EXTENDED);
  if (status_code != 0) {
    char error_buffer[128];
    regerror(status_code, &regex, error_buffer, sizeof(error_buffer));
    raise_critical_error(ERROR_RUNTIME, "Regex compilation failed: %s", error_buffer);
  }

  status_code = regexec(&regex, filepath, 2, match, 0);
  if (status_code != 0) {
    char error_buffer[128];
    regerror(status_code, &regex, error_buffer, sizeof(error_buffer));
    raise_critical_error(ERROR_RUNTIME, "Regex match filed: %s", error_buffer);
  }

  int start = match[1].rm_so;
  int end = match[1].rm_eo;

  int length = end - start;
  char * extension = (char *)malloc(length + 1); // +1 for '\0'
  if (!extension) raise_critical_error(ERROR_RUNTIME, "Error allocating memory for file extension");

  memcpy(extension, filepath + start, length);
  extension[length] = '\0';

  regfree(&regex);

  return extension;
}

bool is_video(const char * file_extension) {
  if (!file_extension) raise_critical_error(ERROR_BAD_ARGUMENTS, "file_extension is null pointer");
  if (strcmp(file_extension, "mp4") == 0) return true;
  return false;
}

bool is_image(const char * file_extension) {
  static const char * ImageExtensionArray[] = {"jpg", "jpeg", "png", "bmp", "psd", "tga", "gif", "hdr", "pic", "ppm", "pgm"};
  size_t sz = 11;  // size of ImageExtensionArray

  for (size_t i = 0; i < sz; ++i) {
    if (strcmp(file_extension, ImageExtensionArray[i]) == 0) return true;
  }

  return false;
}