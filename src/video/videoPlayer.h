#pragma once

#include "../image/ImageStbi.h"

#include <time.h>
#include <stdio.h>

typedef struct {
  size_t width;
  size_t height;
} videoDimensions;

static videoDimensions get_video_resolution(const char * filename);
// write video resolution into stbi->width and stbi->height using ffprobe

static double get_video_fps(const char * filename);
// get fps of a video using ffprobe

static double calculate_timestamp(const size_t current_frame, const double video_fps);
// return amount of seconds passed after current_frame shown frames
// return is floor divisioned by TIMESTAMP_ROUNDING_PRECISION

static FILE * open_ffmpeg_pipeline(const char * filepath, const double timestamp);
// open 3 bit ffpmeg frame pipeline 
// each bit is a rgb channel

static void sleep_frame_time_offset(const struct timespec * start, const struct timespec * end, const double FPS);
// sleep until the next frame should be displayed
// start and end are the times of the current frame processing

static ImageStbi * create_frame(const char * filepath);
// create stbi object to read rgb frames into

static void pause_playback(FILE * pipe);

static size_t read_frame(ImageStbi * stbi, FILE * pipe);
// return amount of bytes read from pipe

static void print_ui(const ImageStbi * stbi, const size_t block_sz);

static void print_frame(const ImageStbi * stbi,  const size_t block_sz, const size_t fps);

void play_video(const char * filepath, const size_t block_sz);
// convert a mp4 video into a sequence of frames in "frames" folder
// and print them all out frame by frame in the terminal in ascii format