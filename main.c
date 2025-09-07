#include "src/imageToAscii.h"
#include "src/videoToAscii.h"
#include "src/Config.h"
#include "src/terminalInput.h"
#include "src/error.h"

#include "stdlib.h"

int main(int argc, char ** argv) {
  /*-----------------------Load parameters from terminal-----------------------*/
  OPTIONS config = loadDefaultConfig();
  readTerminalArguments(&config, argc, argv);

  char * fileExtension = filenameExtension(config.filename);
  if (!fileExtension) printCriticalError(ERROR_BAD_ARGUMENTS, "No file extension found in %s", config.filename);
  else printf("File Extension: %s\n", fileExtension);

  /*-----------------------Execute code-----------------------*/

  if (isVideo(fileExtension)) printVideo(config.filename, config.blockSize, config.FPS);
  if (isImage(fileExtension)) printImage(config.filename, config.blockSize);

  free(fileExtension);

  return SUCCESS;
}