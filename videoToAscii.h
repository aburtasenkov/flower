#pragma once

#include "imageToAscii.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>

#define FFMPEG_DECOMPOSE_VIDEO "ffmpeg -i %s -r 24 frames/frame_%%04d.png"
// --> max amount of frames = 9999
#define MAX_FRAMES 9999

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

bool fileExists(const char * filename) {
  if (filename == NULL) {
    printf("Pre-condition fileExists(const char * filename): filename is null pointer\n");
    return false;
  }

  if (access(filename, F_OK) == 0) {
    return true;
  }
  return false;
}

void calculateFrameTimeOffset(int FPS, struct timespec& start, struct timespec& end) 
// sleep until the next frame should be displayed
// start and end are the times of the current frame processing
{
  if (start.tv_sec > end.tv_sec || (start.tv_sec == end.tv_sec && start.tv_nsec > end.tv_nsec)) {
    printf("Pre-condition calculateFrameTimeOffset(struct timespec& start, struct timespec& end, double& elapsedTime): start time is after end time\n");
    return;
  }

  double frameTime = 1.0 / FPS;

  elapsedTime = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
  double sleepTime = frameTime - elapsedTime;

  if (sleepTime > 0) {
    struct timespec sleepDuration;
    sleepDuration.tv_sec = (time_t)sleepTime;
    sleepDuration.tv_nsec = (long)((sleepTime - sleepDuration.tv_sec) * 1e9);
    nanosleep(&sleepDuration, NULL);
  }
}

void printFrames(int blockSize) {
  if (blockSize < 1) {
    printf("Pre-condition printFrames(int blockSize): blockSize is smaller than 1\n");
    return;
  }
  char filepath[22];
  
  int FPS = 60;

  struct timespec start, end;

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

    calculateFrameTimeOffset(start, end); // sleep until the next frame should be displayed
  }
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

  if (executeCommand("mkdir frames") != 0) 
  {
    printf("Error creating frames directory. Aborting...\n");
    free(ffmpegCommand);
    return;
  }
  if (executeCommand(ffmpegCommand) != 0) 
  {
    printf("Error executing ffmpeg command: %s. Aborting...\n", ffmpegCommand);
    free(ffmpegCommand);
    return;
  }
  if (executeCommand("clear") != 0) // NON PORTABLE COMMAND
  {
    printf("Error clearing terminal. Aborting...\n");
    free(ffmpegCommand);
    return;
  }

  printFrames(blockSize);

  if (executeCommand("rm -rf frames") != 0) {
    printf("Error removing frames directory.\n");
  }
  free(ffmpegCommand);
}