#include "src/ascii/stbiToAscii.h"
#include "src/video/videoToAscii.h"
#include "src/config/Config.h"
#include "src/io/terminalInput.h"
#include "src/error.h"
#include "src/io/writeFile.h"

#include "stdlib.h"

int main(int argc, char ** argv) {
  /*-----------------------Load parameters from terminal-----------------------*/
  OPTIONS config = loadDefaultConfig();
  readTerminalArguments(&config, argc, argv);

  char * extension = fileExtension(config.filepath);
  printf("File Extension: %s\n", extension);

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