#include "videoToAscii.h"

#include "../image/ImageStbi.h"
#include "../ascii/stbiToAscii.h"

#include "../error.h"

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

// #define DEBUG

static int execute_command(const char * command) 
// Wrapper for system(*command*) calls in order to ease debugging
{
  int status_code = system(command);

  // check if system failed
  if (status_code == -1) printCriticalError(ERROR_RUNTIME, "execute_command(\"%s\") failed", command);

  // it exited normally --> check exit code
  if (WIFEXITED(status_code)) {
    int exit_code = WEXITSTATUS(status_code);

    if (exit_code == EXIT_SUCCESS) {
      return EXIT_SUCCESS;
    }

    printNonCriticalError(exit_code, "\"%s\" ran unsucessfully (exit code %d).\n", command, exit_code);
    return exit_code;
  }
  // check for signal termination
  if (WIFSIGNALED(status_code)) {
    int sig = WTERMSIG(status_code);
    sig += 128; // common convention for signal termination
    printNonCriticalError(sig, "\"%s\" was terminated by signal %d.\n", command, sig);
    return sig;
  }
  // all other non formal terminations
  printCriticalError(ERROR_RUNTIME, "system(\"%s\") did not terminate normally", command);
  return 1;
}

static bool file_exists(const char * filepath) {
  if (access(filepath, F_OK) == 0) {
    return true;
  }
  return false;
}

static void sleep_frame_time_offset(size_t FPS, const timespec_t * start, const timespec_t * end) 
// sleep until the next frame should be displayed
// start and end are the times of the current frame processing
{
  if (!start || !end) printCriticalError(ERROR_BAD_ARGUMENTS, "start or end is null pointer");
  if (start->tv_sec > end->tv_sec || (start->tv_sec == end->tv_sec && start->tv_nsec > end->tv_nsec))
    printCriticalError(ERROR_BAD_ARGUMENTS, "start time is after end time");

  double frame_time = 1.0 / FPS;
  double elapsed_time = (end->tv_sec - start->tv_sec) + (end->tv_nsec - start->tv_nsec) / NANOSECONDS_IN_SECOND;
  double sleep_time = frame_time - elapsed_time;

  if (sleep_time > 0) {
    timespec_t sleep_duration;
    sleep_duration.tv_sec = (time_t)sleep_time;
    sleep_duration.tv_nsec = (long)((sleep_time - sleep_duration.tv_sec) * NANOSECONDS_IN_SECOND);
    nanosleep(&sleep_duration, NULL);
  }
}

static void print_frames(size_t block_sz, size_t FPS) 
// print each frame of a video while accounting for wished FPS
{
  char filepath[22];
  bool UNLIMITED_FPS = FPS == 0 ? true : false;
  
  // count frames and cache them
  size_t capacity = 128;
  size_t sz = 0;
  char ** ascii_frames = (char **)malloc(capacity * sizeof(char *));
  
  for (size_t i = 1; i < MAX_FRAMES; ++i)
  {
    snprintf(filepath, sizeof(filepath), "frames/frame_%04zu.jpg", i);
    if (!file_exists(filepath)) break;

    printf("\033[HProcessing frame #%04zu\n", i);
    
    if (sz >= capacity) 
    {
      capacity *= 2;
      ascii_frames = (char **)realloc(ascii_frames, capacity * sizeof(char *));
    }

    ImageStbi * stbi = load_stbi(filepath);
    char * ascii = stbi_to_ascii(stbi, block_sz);
    free_stbi(stbi);

    ascii_frames[sz++] = ascii;
  }
    
// if debugging -> print real fps shown
#ifdef DEBUG
timespec_t t0, t1;
clock_gettime(CLOCK_MONOTONIC, &t0);
#endif

  // draw individual frames
  timespec_t start, end;
  for (size_t i = 0; i < sz; ++i) {
    clock_gettime(CLOCK_MONOTONIC, &start);

    // Move cursor to the top left corner of the terminal and print the frame
    printf("\033[H%s", ascii_frames[i]);

    clock_gettime(CLOCK_MONOTONIC, &end);

    if (!UNLIMITED_FPS) {
      sleep_frame_time_offset(FPS, &start, &end); // sleep until the next frame should be displayed
    }
  }

#ifdef DEBUG
clock_gettime(CLOCK_MONOTONIC, &t1);
double elapsed = (t1.tv_sec - t0.tv_sec) + (t1.tv_nsec - t0.tv_nsec) / NANOSECONDS_IN_SECOND;

double fps = (double)sz / elapsed;
printf("Displayed %zu frames in %.3f seconds (%.2f FPS)\n", sz, elapsed, fps);
#endif

  // free all frames
  for (size_t i = 0; i < sz; ++i) 
  {
    free(ascii_frames[i]);
  }
  free(ascii_frames);
}

void print_video(const char * filepath, size_t block_sz, size_t FPS) 
// convert a mp4 video into a sequence of frames in "frames" folder
// and print them all out frame by frame in the terminal in ascii format
{
  if (!filepath) printCriticalError(ERROR_BAD_ARGUMENTS, "filepath is null pointer");
  if (block_sz < 1) printCriticalError(ERROR_BAD_ARGUMENTS, "block_sz is smaller than 1 [block_sz: %zu]", block_sz);

  size_t ffmpeg_command_sz = strlen(FFMPEG_DECOMPOSE_VIDEO) + strlen(filepath) + 1;
  char * ffmpeg_command = (char *)malloc(ffmpeg_command_sz);
  if (!ffmpeg_command) printCriticalError(ERROR_RUNTIME, "Can not allocate enough memory for ffmpeg_command [size in bytes: %zu]", ffmpeg_command_sz);
  
  snprintf(ffmpeg_command, ffmpeg_command_sz, FFMPEG_DECOMPOSE_VIDEO, filepath);

  if (execute_command("mkdir frames") != 0) 
  {
    printNonCriticalError(ERROR_RUNTIME, "Can not create \"frames\" directory");
    free(ffmpeg_command);
    return;
  }
  if (execute_command(ffmpeg_command) != 0) 
  {
    printNonCriticalError(ERROR_RUNTIME, "Error executing ffmpeg command: %s", ffmpeg_command);
    free(ffmpeg_command);
    return;
  }
  if (execute_command(clearCommand) != 0)
  {
    printNonCriticalError(ERROR_RUNTIME, "Can not clear terminal [command: %s]", clearCommand);
    free(ffmpeg_command);
    return;
  }

  print_frames(block_sz, FPS);

  if (execute_command("rm -rf frames") != 0)
  {
    printNonCriticalError(ERROR_RUNTIME, "Can not remove \"frames\" directory");
  }
  free(ffmpeg_command);
}