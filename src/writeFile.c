#include "writeFile.h"

#include "ImageStbi.h"
#include "stbiToAscii.h"
#include "asciiToPpm.h"
#include "terminalInput.h"
#include "error.h"

#include <string.h>

void writeImage(const char * filepath, const char * outputPath, size_t blockSize)
// convert image at filepath to ascii and write the resulting image in outputPath
// if file format is not supported, function will cause an error
{
  if (!filepath) printCriticalError(ERROR_BAD_ARGUMENTS, "filepath is null pointer");
  if (!outputPath) printCriticalError(ERROR_BAD_ARGUMENTS, "outputPath is null pointer");
  if (blockSize < 1) printCriticalError(ERROR_BAD_ARGUMENTS, "blockSize < 1 [blockSize: %zu]", blockSize);

  char * extension = fileExtension(outputPath);

  // check if output file format is supported
  if (strcmp(extension, "ppm") == 0) 
  {
    free(extension);
    writePpm(filepath, outputPath, blockSize);
    return;
  }
  if (strcmp(extension, "txt") == 0) 
  {
    free(extension);
    writeTxt(filepath, outputPath, blockSize);
    return;
  }

  printCriticalError(ERROR_BAD_ARGUMENTS, "File format '%s' is not supported", extension);
  //didn't forget "free(extension);", its just not needed as printCriticalError will stop control flow and OS will cleanup
}

static void writePpm(const char * filepath, const char * outputPath, size_t blockSize)
// convert image at filepath to ppm file
{
  // load stbi image and convert it to ascii
  ImageStbi * stbi = loadStbi(filepath);
  unsigned char * asciiImage = stbiToAscii(stbi, blockSize);
  freeStbi(stbi);
  
  asciiToPpm(asciiImage, outputPath);
  free(asciiImage);
}

static void writeTxt(const char * filepath, const char * outputPath, size_t blockSize)
// convert image at filepath to txt file
{
  // load stbi image and convert it to ascii image
  ImageStbi * stbi = loadStbi(filepath);
  unsigned char * asciiImage = stbiToAscii(stbi, blockSize);
  freeStbi(stbi);
  
  FILE * txt = fopen(outputPath, "w");
  fprintf(txt, "%s", asciiImage);
  if (fclose(txt) != 0) 
  {
    perror("fclose");
  }
  
  free(asciiImage);
}