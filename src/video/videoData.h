#pragma once

#include "../image/ImageStbi.h"

#include <stdio.h>

typedef struct {
  size_t width;
  size_t height;
} videoDimensions;

videoDimensions get_video_resolution(const char * filename);
// write video resolution into stbi->width and stbi->height using ffprobe

double get_video_fps(const char * filename);
// get fps of a video using ffprobe

double calculate_timestamp(const size_t current_frame, const double video_fps);
// return amount of seconds passed after current_frame shown frames
// return is floor divisioned by TIMESTAMP_ROUNDING_PRECISION

FILE * open_ffmpeg_pipeline(const char * filepath, const double timestamp);
// open 3 bit ffpmeg frame pipeline 
// each bit is a rgb channel

ImageStbi * create_frame(const char * filepath);
// create stbi object to read rgb frames into

size_t read_frame(FILE * pipe, ImageStbi * stbi);
// return amount of bytes read from pipe
