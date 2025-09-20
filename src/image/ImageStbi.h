#pragma once

#include <stddef.h>

typedef struct {
  unsigned char * data;
  size_t data_sz;
  size_t width;
  size_t height;
  size_t n_components;
} ImageStbi;

ImageStbi * load_stbi(const char * filepath);
// create object of ImageStbi class that loads images in format that is supported by stbi

ImageStbi * create_stbi(const size_t width, const size_t height, const size_t n_components);
// create empty ImageStbi object

void free_stbi(ImageStbi * image);
