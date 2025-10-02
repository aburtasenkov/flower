#pragma once

#include "../image/ImageStbi.h"

#include <stdio.h>

typedef struct {
  size_t width;
  size_t height;
} videoDimensions;

typedef struct {
  videoDimensions dimensions;
  double fps;
} videoData;

static videoDimensions get_video_resolution(const char *);
// write video resolution into stbi->width and stbi->height using ffprobe

static double get_video_fps(const char *);
// get fps of a video using ffprobe

videoData get_video_information(const char *);
// get all infos needed about a video located at filepath

double calculate_timestamp(const size_t, const double);
// return amount of seconds passed after current_frame shown frames
// return is floor divisioned by TIMESTAMP_ROUNDING_PRECISION

FILE * open_ffmpeg_pipeline(const char *, const double);
// open 3 bit ffpmeg frame pipeline 
// each bit is a rgb channel

ImageStbi * create_frame(const char *);
// create stbi object to read rgb frames into

size_t read_frame(FILE *, ImageStbi *);
// return amount of bytes read from pipe
