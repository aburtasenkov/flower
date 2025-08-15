#include "imageToAscii.h"

#include <stdlib.h>
#include <regex.h>
#include <sys/wait.h>

#define ARGC_MIN 2
#define IMAGE_PATH_ARGV_INDEX 1
#define BLOCKSIZE_ARGV_INDEX 2

#define REGEX_FILE_EXTENSION_PATTERN "\\.([^.]+)$"
#define FFMPEG_DECOMPOSE_VIDEO "ffmpeg -i %s -r 24 frames/frame_%%04d.png"

// TODO
// PRE CONDITIONS FOR FUNCTIONS
// POST CONDITIONS FOR ALL FUNCTIONS

void printImage(const char * filename, int blockSize) 
// convert image located at filename into ascii character array and print it out in terminal
{
  // Load image
  int width, height, Ncomponents;
  unsigned char * image = stbi_load(filename, &width, &height, &Ncomponents, 0);
  if (!image) {
    printf("Error loading image. Aborting...\n");
    return;
  }

  printf("Read image width:%d Height:%d ComponentsSize:%d\n", width, height, Ncomponents);
  char * asciiImage = createAsciiImage(image, width, height, Ncomponents, blockSize);
  if (!asciiImage) {
    printf("Error converting image to ascii. Aborting...\n");
    return;
  }
  printf("%s\n", asciiImage);

  stbi_image_free(image);
  free(asciiImage);
}

char * filenameExtension(const char * filename)
// return the file extension of filename variable
{
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

int executeCommand(const char * command) 
// Wrapper for system(*command*) calls in order to ease debugging
{
  if (command == NULL) {
    printf("Pre-condition executeCommand(const char * command): command is null pointer\n");
    return 1;
  }

  printf("Running command: %s\n", command);
  int statusCode = system(command);

  // check if system failed
  if (statusCode == -1) {
    printf("system(\"%s\") failed. Aborting...\n", command);
    return 1;
  }

  // it exited normally --> check exit code
  if (WIFEXITED(statusCode)) {
    int exitCode = WEXITSTATUS(statusCode);

    if (exitCode == 0) {
      printf("\"%s\" ran successfully.\n", command);
      return 0;
    }

    printf("\"%s\" ran unsucessfully (exit code %d).\n", command, exitCode);
    return exitCode;
  }


  // check for signal termination
  if (WIFSIGNALED(statusCode)) {
    int sig = WTERMSIG(statusCode);
    printf("\"%s\" was terminated by signal %d.\n", command, sig);
    return 128 + sig;  // common convention for signal termination
  }

  // all other non formal terminations
  printf("system(\"%s\") did not terminate normally. Aborting...\n", command);
  return 1;
}

void printVideo(const char * filename, int blockSize) 
// convert a mp4 video into a sequence of frames in "frames" folder
// and print them all out frame by frame in the terminal in ascii format
{
  if (filename == NULL) {
    printf("Pre-condition void printVideo(const char * filename, int blockSize): filename is null pointer\n");
    return;
  }

  size_t ffmpegCommandSize = strlen(FFMPEG_DECOMPOSE_VIDEO) + strlen(filename) + 1;
  char * ffmpegCommand = (char *)malloc(ffmpegCommandSize);
  if (ffmpegCommand == NULL) {
    printf("Error allocating memory for ffmpegCommand in printVideo(const char * filename, int blockSize). Aborting...\n");
    return;
  }
  snprintf(ffmpegCommand, ffmpegCommandSize, FFMPEG_DECOMPOSE_VIDEO, filename);

  if (executeCommand("mkdir frames") != 0) return;
  if (executeCommand(ffmpegCommand) != 0) return;
  if (executeCommand("clear") != 0) return; // NON PORTABLE COMMAND

  // PRINTING FRAMES

  if (executeCommand("rm -rf frames") != 0) return;
  
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