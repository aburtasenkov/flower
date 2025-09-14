#pragma once

#include <stddef.h>
#include <stdbool.h>

#define FFMPEG_DECOMPOSE_VIDEO "ffmpeg -i %s -r 24 frames/frame_%%04d.jpg"
// --> max amount of frames = 9999
#define MAX_FRAMES 9999

bool file_exists(const char * filepath);

char * file_extension(const char * filepath);
// return the file extension of filepath variable

void write_image(const char * filepath, const char * output_path, size_t block_sz);
// convert image at filepath to ascii and write the resulting image in output_path
// if file format is not supported, function will cause an error

static void write_ppm(const char * filepath, const char * output_path, size_t block_sz);
// convert image at filepath to ppm file

static void write_txt(const char * filepath, const char * output_path, size_t block_sz);
// convert image at filepath to txt file
