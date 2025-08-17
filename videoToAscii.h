#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <cstring>

#define FFMPEG_DECOMPOSE_VIDEO "ffmpeg -i %s -r 24 frames/frame_%%04d.png"

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

  if (blockSize < 1) {
    printf("Pre-condition void printVideo(const char * filename, int blockSize): blockSize is smaller than 1");
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