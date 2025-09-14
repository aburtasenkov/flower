#include "ImageStbi.h"

#include "../error.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

ImageStbi * load_stbi(const char * filepath)
// create object of ImageStbi class that loads images in format that is supported by stbi
{
  if (!filepath) raise_critical_error(ERROR_BAD_ARGUMENTS, "filepath is null pointer");

  ImageStbi * stbi = (ImageStbi *)malloc(sizeof(ImageStbi));
  if (!stbi) raise_critical_error(ERROR_RUNTIME, "Cannot allocate enough memory for stbi Image [size in bytes: %zu]", sizeof(ImageStbi));

  int width, height, n_components;
  unsigned char * data = stbi_load(filepath, &width, &height, &n_components, 0);
  if (!data) raise_critical_error(ERROR_RUNTIME, "Cannot load image [filepath: %s]", filepath);

  stbi->data = data;
  stbi->width = (size_t)width;
  stbi->height = (size_t)height;
  stbi->n_components = (size_t)n_components;

  return stbi;
}

void free_stbi(ImageStbi * stbi) {
  if (!stbi) return;
  if (!stbi->data) { free(stbi); return; }

  stbi_image_free(stbi->data);
  free(stbi);
}