#include "stbiToAscii.h"

#include "../error.h"

#include <assert.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>

static char grayscale_ascii_map[UINT8_MAX + 1] = {0};

void init_grayscale_ascii_map()
// create hash map for each value to avoid recalculating ascii characters with same grayscale value
{
  for (size_t i = 0; i <= UINT8_MAX; ++i) {
    grayscale_ascii_map[i] = compute_grayscale_Char((uint8_t)i);
    printf("%c\n", grayscale_ascii_map[i]);
  }
}

static inline char compute_grayscale_Char(uint8_t grayscale)
// map a value of 0-255 on to an array of grayscale characters and return the character
{
  static const char ascii[] = "$@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/\\|()1{}[]?-_+~<>i!lI;:,\"^`'. ";
  static const size_t sz = sizeof(ascii) - 1;

  // flatten [0:255] values on [0:sz[ ids
  size_t index = (size_t)((float)grayscale / 255.0f * (sz - 1));

  return ascii[index];
}

static inline uint8_t rgb_grayscale_value(const unsigned char * pixel, size_t n_components) 
// convert single or multichannel pixel to grayscale value [0:255]
{
  static const float RGB_FACTORS[] = {0.299f, 0.587f, 0.114f};
  
  if (n_components == 1 || n_components == 2) return pixel[0];

  float grayscale = 0.0f;
  for (size_t i = 0; i < 3; ++i) {
    grayscale += pixel[i] * RGB_FACTORS[i];
  }
  return (uint8_t)grayscale;
}

static inline char grayscale_char(uint8_t grayscale)
// retrieve pre computed ascii characters
{
  return grayscale_ascii_map[grayscale];
}

char * stbi_to_ascii(const ImageStbi * stbi, size_t block_sz) 
// create array of ascii characters
// this array includes \n and \o for newlines and end of string
// no checks and therefore the caller must ensure stbi is valid pointer
{
  size_t out_width = (stbi->width + block_sz - 1) / block_sz;
  size_t out_height = (stbi->height + block_sz - 1) / block_sz;

  char * ascii_image = (char *)malloc(out_width * out_height + out_height + 1); // +height for "\n" and +1 for "\0" characters
  if (!ascii_image) raise_critical_error(ERROR_RUNTIME, "Can not allocate enough memory for ascii_image [size in bytes: %zu]", out_width * out_height + out_height + 1);
  size_t idx = 0;

  // iterate over each pixel
  for (size_t by = 0; by < stbi->height; by += block_sz) {
    for (size_t bx = 0; bx < stbi->width; bx += block_sz) {

      size_t sum = 0;
      size_t count = 0;

      for (size_t y = 0; y < block_sz && (by + y) < stbi->height; ++y) {
        for (size_t x = 0; x < block_sz && (bx + x) < stbi->width; ++x) {
          unsigned char * current_pixel = stbi->data + ((by + y) * stbi->width + bx + x) * stbi->n_components;

          uint8_t grayscale = rgb_grayscale_value(current_pixel, stbi->n_components);
          sum += grayscale;
          ++count;
        }
      }

      uint8_t grayscale_average = sum / count;
      ascii_image[idx++] = grayscale_char(grayscale_average);
    }
    ascii_image[idx++] = '\n';
  }
  ascii_image[idx] = '\0';
  return ascii_image;
}

void print_image(const char * filepath, size_t block_sz) 
// convert image located at filepath into ascii character array and print it out in terminal
{
  if (!filepath) raise_critical_error(ERROR_BAD_ARGUMENTS, "filepath is null pointer");

  ImageStbi * stbi = load_stbi(filepath);
  printf("Read image width:%zu Height:%zu ComponentsSize:%zu\n", stbi->width, stbi->height, stbi->n_components);

  printf("Converting image %s into ascii grayscale array\n", filepath);
  char * ascii_image = stbi_to_ascii(stbi, block_sz);
  free_stbi(stbi);

  printf("%s", ascii_image);
  free(ascii_image);
}
