#include "videoToAscii.h"

#include "../image/ImageStbi.h"
#include "../ascii/stbiToAscii.h"
#include "../io/files.h"
#include "../io/terminal.h"

#include "../error.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define NANOSECONDS_IN_SECOND 1e9

void print_video(const char * filepath, size_t block_sz, size_t FPS) {
    if (!filepath) raise_critical_error(ERROR_BAD_ARGUMENTS, "filepath is null pointer");
    if (block_sz < 1) raise_critical_error(ERROR_BAD_ARGUMENTS, "block_sz must be >= 1");

    char cmd[512];
    snprintf(cmd, sizeof(cmd),
             "ffmpeg -loglevel quiet -i \"%s\" -f rawvideo -pix_fmt rgb24 -",
             filepath);

    FILE *pipe = popen(cmd, "r");
    if (!pipe) raise_critical_error(ERROR_RUNTIME, "popen failed running ffmpeg");

    // We'll need to know the original video width/height. 
    // You can get it with ffprobe or set it manually if fixed
    int video_w = 1920; // replace with actual width
    int video_h = 1080; // replace with actual height

    size_t frame_size = video_w * video_h * 3;
    unsigned char *buffer = (unsigned char *)malloc(frame_size);
    if (!buffer) raise_critical_error(ERROR_RUNTIME, "cannot allocate frame buffer");

    ImageStbi stbi;
    stbi.width = video_w;
    stbi.height = video_h;
    stbi.n_components = 3;
    stbi.data = buffer;

    timespec_t frame_start, frame_end, start, end;
    size_t total_fps_shown = 0;

    clock_gettime(CLOCK_MONOTONIC, &start);

    while (fread(buffer, 1, frame_size, pipe) == frame_size) {
        clock_gettime(CLOCK_MONOTONIC, &frame_start);

        char *ascii_frame = stbi_to_ascii(&stbi, block_sz);

        printf("\033[H\033[J%s", ascii_frame);
        fflush(stdout);

        free(ascii_frame);

        clock_gettime(CLOCK_MONOTONIC, &frame_end);
        sleep_frame_time_offset(FPS, &frame_start, &frame_end);
        total_fps_shown++;
    }
    
    free(buffer);
    pclose(pipe);

    clock_gettime(CLOCK_MONOTONIC, &end);

    double elapsed = (end.tv_sec - start.tv_sec)
                     + (end.tv_nsec - start.tv_nsec) / 1e9;

    if (elapsed > 0.0) {
        double avg_fps = total_fps_shown / elapsed;
        printf("\nAverage FPS: %.2f (%zu frames in %.3f seconds)\n",
               avg_fps, total_fps_shown, elapsed);
    }
}

static void sleep_frame_time_offset(size_t FPS, const timespec_t * start, const timespec_t * end) 
// sleep until the next frame should be displayed
// start and end are the times of the current frame processing
{
  if (!start || !end) raise_critical_error(ERROR_BAD_ARGUMENTS, "start or end is null pointer");
  if (start->tv_sec > end->tv_sec || (start->tv_sec == end->tv_sec && start->tv_nsec > end->tv_nsec))
    raise_critical_error(ERROR_BAD_ARGUMENTS, "start time is after end time");

  double frame_time = 1.0 / FPS;
  double elapsed_time = (end->tv_sec - start->tv_sec) + (end->tv_nsec - start->tv_nsec) / NANOSECONDS_IN_SECOND;
  double sleep_time = frame_time - elapsed_time;

  if (sleep_time > 0) {
    timespec_t sleep_duration;
    sleep_duration.tv_sec = (time_t)sleep_time;
    sleep_duration.tv_nsec = (long)((sleep_time - sleep_duration.tv_sec) * NANOSECONDS_IN_SECOND);
    nanosleep(&sleep_duration, NULL);
  }
}