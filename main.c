#include "src/imageToAscii.h"
#include "src/videoToAscii.h"
#include "src/Config.h"
#include "src/terminalInput.h"
#include "src/error.h"
#include "src/writeFile.h"

#include "stdlib.h"

int main(int argc, char ** argv) {
  /*-----------------------Load parameters from terminal-----------------------*/
  OPTIONS config = loadDefaultConfig();
  readTerminalArguments(&config, argc, argv);

  char * extension = fileExtension(config.filepath);
  if (!extension) printCriticalError(ERROR_BAD_ARGUMENTS, "No file extension found in %s", config.filepath);
  else printf("File Extension: %s\n", extension);

  /*-----------------------Execute code-----------------------*/

  if (isVideo(extension)) 
  {
    printVideo(config.filepath, config.blockSize, config.FPS);
  }
  else if (isImage(extension))
  {
    if (config.output) {
      writeImage(config.filepath, config.output, config.blockSize);
      printf("Wrote ascii image to file: %s\n", config.output);
    }
    else printImage(config.filepath, config.blockSize);
  }

  free(extension);

  return EXIT_SUCCESS;
}