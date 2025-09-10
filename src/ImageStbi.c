#include "ImageStbi.h"

#include "error.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

ImageStbi * loadStbi(const char * filepath)
// create object of ImageStbi class that loads images in format that is supported by stbi
{
  if (!filepath) printCriticalError(ERROR_BAD_ARGUMENTS, "filepath is null pointer");

  ImageStbi * stbi = (ImageStbi *)malloc(sizeof(ImageStbi));
  if (!stbi) printCriticalError(ERROR_RUNTIME, "Cannot allocate enough memory for stbi Image [size in bytes: %zu]", sizeof(ImageStbi));

  int width, height, Ncomponents;
  unsigned char * data = stbi_load(filepath, &width, &height, &Ncomponents, 0);
  if (!data) printCriticalError(ERROR_RUNTIME, "Cannot load image [filepath: %s]", filepath);

  stbi->data = data;
  stbi->width = (size_t)width;
  stbi->height = (size_t)height;
  stbi->Ncomponents = (size_t)Ncomponents;

  return stbi;
}

void freeStbi(ImageStbi * stbi) {
  if (!stbi) return;
  if (!stbi->data) { free(stbi); return; }

  stbi_image_free(stbi->data);
  free(stbi);
}