#include "terminal.h"

#include "../error.h"

#include <stdio.h>
#include <string.h>

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