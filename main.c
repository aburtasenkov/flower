#include "src/ascii/stbiToAscii.h"
#include "src/video/videoToAscii.h"
#include "src/config/Config.h"
#include "src/io/terminal.h"
#include "src/error.h"
#include "src/io/files.h"

#include "stdlib.h"

int main(int argc, char ** argv) {
  /*-----------------------initialize optimized grayscale to ascii converter----------------*/
  init_grayscale_ascii_map();

  /*-----------------------Load parameters from terminal-----------------------*/
  OPTIONS config = default_config();
  parse_options(&config, argc, argv);

  char * extension = file_extension(config.filepath);
  printf("File Extension: %s\n", extension);

  /*-----------------------Execute code-----------------------*/

  if (is_video(extension)) 
  {
    print_video(config.filepath, config.block_sz);
  }
  else if (is_image(extension))
  {
    if (config.output) {
      write_image(config.filepath, config.output, config.block_sz);
      printf("Wrote ascii image to file: %s\n", config.output);
    }
    else print_image(config.filepath, config.block_sz);
  }

  free(extension);

  return EXIT_SUCCESS;
}