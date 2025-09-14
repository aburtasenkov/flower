#pragma once

#include <stddef.h>

typedef struct {
  unsigned char * data;
  size_t width;
  size_t height;
  size_t n_components;
} ImageStbi;

ImageStbi * load_stbi(const char * filepath);
// create object of ImageStbi class that loads images in format that is supported by stbi

void free_stbi(ImageStbi * image);
