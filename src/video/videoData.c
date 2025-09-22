#include "videoData.h"

#include "../error.h"

#include <stdlib.h>
#include <math.h>

#define FFPROBE_RESOLUTION_COMMAND "ffprobe -v error -select_streams v:0 -show_entries stream=width,height -of csv=p=0:s=x \"%s\""
#define FFPROBE_FPS_COMMAND "ffprobe -v error -select_streams v:0 -show_entries stream=r_frame_rate -of default=noprint_wrappers=1:nokey=1 \"%s\""

#define FFMPEG_COMMAND "ffmpeg -loglevel quiet -ss %.3f -i \"%s\" -f rawvideo -pix_fmt rgb24 -"

#define TIMESTAMP_ROUNDING_PRECISION 3

videoDimensions get_video_resolution(const char * filepath)
// write video resolution into stbi->width and stbi->height using ffprobe
{
  char command[1024];
  char buffer[128];
  videoDimensions dimensions;

  snprintf(command, sizeof(command), FFPROBE_RESOLUTION_COMMAND, filepath);

  FILE * pipe = popen(command, "r");
  if (!pipe)
  {
    raise_critical_error(ERROR_RUNTIME, "popen failed running ffprobe resolution command");
  }

  if (fgets(buffer, sizeof(buffer), pipe))
  {
    sscanf(buffer, "%zux%zu", &dimensions.width, &dimensions.height);
  }
  pclose(pipe);

  return dimensions;
}

double get_video_fps(const char * filepath)
// get fps of a video using ffprobe
{
  char command [1024];
  char buffer[64];
  int num, denom;
  double fps = 0;

  snprintf(command, sizeof(command), FFPROBE_FPS_COMMAND, filepath);

  FILE * pipe = popen(command, "r");
  if (!pipe)
  {
    raise_critical_error(ERROR_RUNTIME, "popen failed running ffprobe fps command");
  }

  if (fgets(buffer, sizeof(buffer), pipe))
  {
    sscanf(buffer, "%d/%d", &num, &denom);
    if (num > 0 && denom > 0)
    {
      fps = (double)num / denom;
    }
  }

  pclose(pipe);
  
  return fps;
}

videoData get_video_information(const char * filepath)
// get all infos needed about a video located at filepath
{
  videoData data = {0};
  data.dimensions = get_video_resolution(filepath);
  data.fps = get_video_fps(filepath);
  return data;
}

double calculate_timestamp(const size_t current_frame, const double video_fps)
// return amount of seconds passed after current_frame shown frames
// return is floor divisioned by TIMESTAMP_ROUNDING_PRECISION
{
  double seconds = (double)current_frame / video_fps;
  double factor = pow(10.0, TIMESTAMP_ROUNDING_PRECISION);
  return round(seconds * factor) / factor;
}

FILE * open_ffmpeg_pipeline(const char * filepath, const double timestamp)
// open 3 bit ffpmeg frame pipeline 
// each bit is a rgb channel
{
  char command[1024];
  snprintf(command, sizeof(command), FFMPEG_COMMAND, timestamp, filepath);

  FILE * pipe = popen(command, "r");
  if (!pipe) raise_critical_error(ERROR_RUNTIME, "popen failed running ffmpeg");

  return pipe;
}

ImageStbi * create_frame(const char * filepath)
// create stbi object to read rgb frames into
{
  videoDimensions dimensions = get_video_resolution(filepath);
  ImageStbi * stbi = create_stbi(dimensions.width, dimensions.height, 3);

  stbi->data = (unsigned char *)malloc(stbi->data_sz);
  if (!stbi->data) raise_critical_error(ERROR_RUNTIME, "cannot allocate frame buffer");  

  return stbi;
}

size_t read_frame(FILE * pipe, ImageStbi * stbi)
// return amount of bytes read from pipe
{
  return fread(stbi->data, 1, stbi->data_sz, pipe);
}