#include "writeFile.h"

#include "../image/ImageStbi.h"
#include "../ascii/stbiToAscii.h"
#include "../ascii/asciiToPpm.h"
#include "terminal.h"
#include "../error.h"

#include <string.h>

void write_image(const char * filepath, const char * output_path, size_t block_sz)
// convert image at filepath to ascii and write the resulting image in output_path
// if file format is not supported, function will cause an error
{
  if (!filepath) printCriticalError(ERROR_BAD_ARGUMENTS, "filepath is null pointer");
  if (!output_path) printCriticalError(ERROR_BAD_ARGUMENTS, "output_path is null pointer");
  if (block_sz < 1) printCriticalError(ERROR_BAD_ARGUMENTS, "block_sz < 1 [block_sz: %zu]", block_sz);

  char * extension = file_extension(output_path);

  // check if output file format is supported
  if (strcmp(extension, "ppm") == 0) 
  {
    free(extension);
    write_ppm(filepath, output_path, block_sz);
    return;
  }
  if (strcmp(extension, "txt") == 0) 
  {
    free(extension);
    write_txt(filepath, output_path, block_sz);
    return;
  }

  printCriticalError(ERROR_BAD_ARGUMENTS, "File format '%s' is not supported", extension);
  //didn't forget "free(extension);", its just not needed as printCriticalError will stop control flow and OS will cleanup
}

static void write_ppm(const char * filepath, const char * output_path, size_t block_sz)
// convert image at filepath to ppm file
{
  // load stbi image and convert it to ascii
  ImageStbi * stbi = load_stbi(filepath);
  char * ascii_image = stbi_to_ascii(stbi, block_sz);
  free_stbi(stbi);
  
  ImagePPM * ppm = ascii_to_ppm(ascii_image);
  free(ascii_image);

  FILE *f = fopen(output_path, "wb");
  if (!f) {
      perror("fopen");
      free_ppm(ppm);
      return;
  }

  fprintf(f, "P6\n%zu %zu\n255\n", ppm->x, ppm->y);
  if (fwrite(ppm->data, 1, ppm->x * ppm->y * RGB_COLOR_CHANNELS, f) != ppm->x * ppm->y * RGB_COLOR_CHANNELS) {
      perror("fwrite");
      fclose(f);
      free_ppm(ppm);
      return;
  }

  if (fclose(f) != 0) {
      perror("fclose");
  }
  free_ppm(ppm);
}

static void write_txt(const char * filepath, const char * output_path, size_t block_sz)
// convert image at filepath to txt file
{
  // load stbi image and convert it to ascii image
  ImageStbi * stbi = load_stbi(filepath);
  char * ascii_image = stbi_to_ascii(stbi, block_sz);
  free_stbi(stbi);
  
  FILE * txt = fopen(output_path, "w");
  fprintf(txt, "%s", ascii_image);
  if (fclose(txt) != 0) 
  {
    perror("fclose");
  }
  
  free(ascii_image);
}