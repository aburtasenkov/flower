#pragma once

#include <stddef.h>

typedef struct {
  unsigned char * data;
  size_t data_sz;
  size_t width;
  size_t height;
  size_t n_components;
} ImageStbi;

ImageStbi * load_stbi(const char *);
// create object of ImageStbi class that loads images in format that is supported by stbi

ImageStbi * create_stbi(const size_t, const size_t, const size_t);
// create empty ImageStbi object

void free_stbi(ImageStbi *);
