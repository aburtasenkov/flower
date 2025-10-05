#include "files.h"

#include "../image/ImageStbi.h"
#include "../ascii/stbiToAscii.h"
#include "../ascii/asciiToPpm.h"
#include "terminal.h"
#include "../error.h"

#include <string.h>
#include <unistd.h>
#include <regex.h>

#define REGEX_FILE_EXTENSION_PATTERN "\\.([^.]+)$"

bool file_exists(const char * filepath) 
{
  if (access(filepath, F_OK) == 0) {
    return true;
  }
  return false;
}

char * file_extension(const char * filepath)
// return the file extension of filepath variable
{
  if (!filepath) raise_critical_error(ERROR_BAD_ARGUMENTS, "filepath is null pointer");

  regex_t regex;
  regmatch_t match[2];

  int status_code = regcomp(&regex, REGEX_FILE_EXTENSION_PATTERN, REG_EXTENDED);
  if (status_code != 0) {
    char error_buffer[128];
    regerror(status_code, &regex, error_buffer, sizeof(error_buffer));
    raise_critical_error(ERROR_RUNTIME, "Regex compilation failed: %s", error_buffer);
  }

  status_code = regexec(&regex, filepath, 2, match, 0);
  if (status_code != 0) {
    char error_buffer[128];
    regerror(status_code, &regex, error_buffer, sizeof(error_buffer));
    raise_critical_error(ERROR_RUNTIME, "Regex match filed: %s", error_buffer);
  }

  int start = match[1].rm_so;
  int end = match[1].rm_eo;

  int length = end - start;
  char * extension = (char *)malloc(length + 1); // +1 for '\0'
  if (!extension) raise_critical_error(ERROR_RUNTIME, "Error allocating memory for file extension");

  memcpy(extension, filepath + start, length);
  extension[length] = '\0';

  regfree(&regex);

  return extension;
}

static void write_ppm(const char * filepath, const char * output_path, const size_t block_sz)
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

static void write_txt(const char * filepath, const char * output_path, const size_t block_sz)
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

void write_image(const char * filepath, const char * output_path, const size_t block_sz)
// convert image at filepath to ascii and write the resulting image in output_path
// if file format is not supported, function will cause an error
{
  if (!filepath) raise_critical_error(ERROR_BAD_ARGUMENTS, "filepath is null pointer");
  if (!output_path) raise_critical_error(ERROR_BAD_ARGUMENTS, "output_path is null pointer");
  if (block_sz < 1) raise_critical_error(ERROR_BAD_ARGUMENTS, "block_sz < 1 [block_sz: %zu]", block_sz);

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
  
  raise_noncritical_error(ERROR_BAD_ARGUMENTS, "File format '%s' is not supported", extension);
  free(extension);
}