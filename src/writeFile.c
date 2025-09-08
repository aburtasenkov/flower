#include "writeFile.h"

#include "imageToAscii.h"
#include "asciiToPpm.h"
#include "terminalInput.h"
#include "error.h"

#include <string.h>

const char const * supportedOutputFormats[] = {"ppm"};

void writeImage(const char * filepath, const char * outputPath, int blockSize)
// convert image at filepath to ascii and write the resulting image in outputPath
// if file format is not supported, function will cause an error
{
  if (!filepath) printCriticalError(ERROR_BAD_ARGUMENTS, "filepath is null pointer");
  if (!outputPath) printCriticalError(ERROR_BAD_ARGUMENTS, "outputPath is null pointer");
  if (blockSize < 1) printCriticalError(ERROR_BAD_ARGUMENTS, "blockSize < 1 [blockSize: %i]", blockSize);

  char * extension = fileExtension(outputPath);

  // check if output file format is supported
  for (int i = 0; i < sizeof(supportedOutputFormats) / sizeof(char *); ++i) {

    if (strcmp(extension, supportedOutputFormats[i]) == SUCCESS) {
      free(extension);

      ImageStbi * stbi = loadStbi(filepath);
      if (!stbi) printCriticalError(ERROR_RUNTIME, "Error loading image [filepath: %s]", filepath);

      unsigned char * asciiImage = stbiToAscii(stbi, blockSize);
      if (!asciiImage) printCriticalError(ERROR_INTERNAL, "Error converting image to ascii format");
      freeStbi(stbi);

      asciiToPpm(asciiImage, outputPath);
      return;
    }
  }

  printCriticalError(ERROR_BAD_ARGUMENTS, "File format '%s' is not supported", extension);
  //didn't forget "free(extension);", its just not needed as printCriticalError will stop control flow
}