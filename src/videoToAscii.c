#include "videoToAscii.h"
#include "imageToAscii.h"

#include "error.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define NANOSECONDS_IN_SECOND 1e9

#define FFMPEG_DECOMPOSE_VIDEO "ffmpeg -i %s -r 24 frames/frame_%%04d.png"
// --> max amount of frames = 9999
#define MAX_FRAMES 9999

// OS dependent command to clear the terminal
#ifdef _WIN32
const char * clearCommand = "cls";
#else
const char * clearCommand = "clear";
#endif

int executeCommand(const char * command) 
// Wrapper for system(*command*) calls in order to ease debugging
{
  if (command == NULL) printCriticalError(ERROR_BAD_ARGUMENTS, "command is null pointer");

  printf("Running command: %s\n", command);
  int statusCode = system(command);

  // check if system failed
  if (statusCode == -1) printCriticalError(ERROR_RUNTIME, "system(\"%s\") failed", command);

  // it exited normally --> check exit code
  if (WIFEXITED(statusCode)) {
    int exitCode = WEXITSTATUS(statusCode);

    if (exitCode == SUCCESS) {
      printf("\"%s\" ran successfully.\n", command);
      return SUCCESS;
    }

    printNonCriticalError(exitCode, "\"%s\" ran unsucessfully (exit code %d).\n", command, exitCode);
    return exitCode;
  }


  // check for signal termination
  if (WIFSIGNALED(statusCode)) {
    int sig = WTERMSIG(statusCode);
    sig += 128; // common convention for signal termination
    printNonCriticalError(sig, "\"%s\" was terminated by signal %d.\n", command, sig);
    return sig;
  }

  // all other non formal terminations
  printCriticalError(ERROR_RUNTIME, "system(\"%s\") did not terminate normally", command);
  return 1;
}

bool fileExists(const char * filepath) {
  if (filepath == NULL) printCriticalError(ERROR_BAD_ARGUMENTS, "filepath is null pointer");

  if (access(filepath, F_OK) == SUCCESS) {
    return true;
  }
  return false;
}

void printFrames(int blockSize, int FPS) 
// print each frame of a video while accounting for wished FPS
{
  if (blockSize < 1) printCriticalError(ERROR_BAD_ARGUMENTS, "blockSize is smaller than 1 [blockSize: %i]", blockSize);
  if (FPS < 0) printCriticalError(ERROR_BAD_ARGUMENTS, "FPS is smaller than 0 [FPS: %i]", FPS);

  char filepath[22];

  timespec_t start, end;

  bool UNLIMITED_FPS = FPS == 0 ? true : false;

  // frames start at 0001
  for (int i = 1; i <= MAX_FRAMES; ++i) {
    clock_gettime(CLOCK_MONOTONIC, &start);

    // Format the filepath for the current frame
    snprintf(filepath, sizeof(filepath), "frames/frame_%04d.png", i);
    if (!fileExists(filepath)) break;

    // Move cursor to the top left corner of the terminal and print the frame
    printf("\033[H");
    printImage(filepath, blockSize);

    clock_gettime(CLOCK_MONOTONIC, &end);

    if (!UNLIMITED_FPS) {
      sleepFrameTimeOffset(FPS, &start, &end); // sleep until the next frame should be displayed
    }
  }
}

void printVideo(const char * filepath, int blockSize, int FPS) 
// convert a mp4 video into a sequence of frames in "frames" folder
// and print them all out frame by frame in the terminal in ascii format
{
  if (filepath == NULL) printCriticalError(ERROR_BAD_ARGUMENTS, "filepath is null pointer");
  if (blockSize < 1) printCriticalError(ERROR_BAD_ARGUMENTS, "blockSize is smaller than 1 [blockSize: %i]", blockSize);
  if (FPS < 0) printCriticalError(ERROR_BAD_ARGUMENTS, "FPS is smaller than 0 [FPS: %i]", FPS);

  size_t ffmpegCommandSize = strlen(FFMPEG_DECOMPOSE_VIDEO) + strlen(filepath) + 1;
  char * ffmpegCommand = (char *)malloc(ffmpegCommandSize);
  if (ffmpegCommand == NULL) printCriticalError(ERROR_INTERNAL, "Can not allocate enough memory for ffmpegCommand [size in bytes: %zu]", ffmpegCommandSize);
  snprintf(ffmpegCommand, ffmpegCommandSize, FFMPEG_DECOMPOSE_VIDEO, filepath);

  if (executeCommand("mkdir frames") != SUCCESS) 
  {
    printNonCriticalError(ERROR_RUNTIME, "Can not create \"frames\" directory");
    free(ffmpegCommand);
    return;
  }
  if (executeCommand(ffmpegCommand) != SUCCESS) 
  {
    printNonCriticalError(ERROR_RUNTIME, "Error executing ffmpeg command: %s", ffmpegCommand);
    free(ffmpegCommand);
    return;
  }
  
  if (executeCommand(clearCommand) != SUCCESS) {
    printNonCriticalError(ERROR_RUNTIME, "Can not clear terminal [command: %s]", clearCommand);
    free(ffmpegCommand);
    return;
  }

  printFrames(blockSize, FPS);

  if (executeCommand("rm -rf frames") != SUCCESS) {
    printNonCriticalError(ERROR_RUNTIME, "Can not remove \"frames\" directory");
  }
  free(ffmpegCommand);
}

void sleepFrameTimeOffset(int FPS, timespec_t * start, timespec_t * end) 
// sleep until the next frame should be displayed
// start and end are the times of the current frame processing
{
  if (FPS < 0) printCriticalError(ERROR_BAD_ARGUMENTS, "FPS is smaller than 0 [FPS: %i]", FPS);
  if (start == NULL) printCriticalError(ERROR_BAD_ARGUMENTS, "start is null pointer");
  if (end == NULL) printCriticalError(ERROR_BAD_ARGUMENTS, "end is null pointer");
  if (start->tv_sec > end->tv_sec || (start->tv_sec == end->tv_sec && start->tv_nsec > end->tv_nsec)) printCriticalError(ERROR_BAD_ARGUMENTS, "start time is after end time");

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