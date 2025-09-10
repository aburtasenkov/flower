#include "videoToAscii.h"

#include "ImageStbi.h"
#include "stbiToAscii.h"

#include "error.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define NANOSECONDS_IN_SECOND 1e9

#define FFMPEG_DECOMPOSE_VIDEO "ffmpeg -i %s -r 24 frames/frame_%%04d.jpg"
// --> max amount of frames = 9999
#define MAX_FRAMES 9999

// OS dependent command to clear the terminal
#ifdef _WIN32
#define clearCommand  "cls"
#else
#define clearCommand "clear"
#endif

static int executeCommand(const char * command) 
// Wrapper for system(*command*) calls in order to ease debugging
{
  int statusCode = system(command);

  // check if system failed
  if (statusCode == -1) printCriticalError(ERROR_RUNTIME, "system(\"%s\") failed", command);

  // it exited normally --> check exit code
  if (WIFEXITED(statusCode)) {
    int exitCode = WEXITSTATUS(statusCode);

    if (exitCode == EXIT_SUCCESS) {
      return EXIT_SUCCESS;
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

static bool fileExists(const char * filepath) {
  if (access(filepath, F_OK) == 0) {
    return true;
  }
  return false;
}

static void sleepFrameTimeOffset(size_t FPS, const timespec_t * start, const timespec_t * end) 
// sleep until the next frame should be displayed
// start and end are the times of the current frame processing
{
  if (!start || !end) printCriticalError(ERROR_BAD_ARGUMENTS, "start or end is null pointer");
  if (start->tv_sec > end->tv_sec || (start->tv_sec == end->tv_sec && start->tv_nsec > end->tv_nsec))
    printCriticalError(ERROR_BAD_ARGUMENTS, "start time is after end time");

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

static void printFrames(size_t blockSize, size_t FPS) 
// print each frame of a video while accounting for wished FPS
{
  char filepath[22];
  timespec_t start, end;
  bool UNLIMITED_FPS = FPS == 0 ? true : false;

  timespec_t START, END;
  size_t frames = 0;
  clock_gettime(CLOCK_MONOTONIC, &START);

  size_t filepathSize = sizeof(filepath);

  // frames start at 0001
  for (size_t i = 1; i <= MAX_FRAMES; ++i) {
    clock_gettime(CLOCK_MONOTONIC, &start);

    // Format the filepath for the current frame
    snprintf(filepath, filepathSize, "frames/frame_%04zu.jpg", i);
    if (!fileExists(filepath)) break;

    // Move cursor to the top left corner of the terminal and print the frame
    printf("\033[H");

    ImageStbi * stbi = loadStbi(filepath);
    char * ascii = stbiToAscii(stbi, blockSize);
    freeStbi(stbi);

    printf("%s", ascii);

    free(ascii);

    clock_gettime(CLOCK_MONOTONIC, &end);

    if (!UNLIMITED_FPS) {
      sleepFrameTimeOffset(FPS, &start, &end); // sleep until the next frame should be displayed
    }
    frames++;
  }
  clock_gettime(CLOCK_MONOTONIC, &END);
  double elapsed = (END.tv_sec - START.tv_sec) +
                   (END.tv_nsec - START.tv_nsec) / NANOSECONDS_IN_SECOND;

  if (elapsed > 0 && frames > 0) {
    double actualFPS = frames / elapsed;
    printf("\n\nDisplayed %zu frames in %.2f seconds (%.2f FPS)\n", frames, elapsed, actualFPS);
  } else {
    printf("\n\nNo frames displayed.\n");
  }
}

void printVideo(const char * filepath, size_t blockSize, size_t FPS) 
// convert a mp4 video into a sequence of frames in "frames" folder
// and print them all out frame by frame in the terminal in ascii format
{
  if (!filepath) printCriticalError(ERROR_BAD_ARGUMENTS, "filepath is null pointer");
  if (blockSize < 1) printCriticalError(ERROR_BAD_ARGUMENTS, "blockSize is smaller than 1 [blockSize: %zu]", blockSize);

  size_t ffmpegCommandSize = strlen(FFMPEG_DECOMPOSE_VIDEO) + strlen(filepath) + 1;
  char * ffmpegCommand = (char *)malloc(ffmpegCommandSize);
  if (!ffmpegCommand) printCriticalError(ERROR_RUNTIME, "Can not allocate enough memory for ffmpegCommand [size in bytes: %zu]", ffmpegCommandSize);
  
  snprintf(ffmpegCommand, ffmpegCommandSize, FFMPEG_DECOMPOSE_VIDEO, filepath);

  if (executeCommand("mkdir frames") != 0) 
  {
    printNonCriticalError(ERROR_RUNTIME, "Can not create \"frames\" directory");
    free(ffmpegCommand);
    return;
  }
  if (executeCommand(ffmpegCommand) != 0) 
  {
    printNonCriticalError(ERROR_RUNTIME, "Error executing ffmpeg command: %s", ffmpegCommand);
    free(ffmpegCommand);
    return;
  }
  if (executeCommand(clearCommand) != 0)
  {
    printNonCriticalError(ERROR_RUNTIME, "Can not clear terminal [command: %s]", clearCommand);
    free(ffmpegCommand);
    return;
  }

  printFrames(blockSize, FPS);

  if (executeCommand("rm -rf frames") != 0)
  {
    printNonCriticalError(ERROR_RUNTIME, "Can not remove \"frames\" directory");
  }
  free(ffmpegCommand);
}