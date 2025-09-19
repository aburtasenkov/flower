# flower - Terminal Multimedia Player

Why is if called flower?

-I wanted to send my girlfriend an ascii flower.

Flower is a C-based tool that converts **images and videos into ASCII**, viewable directly in your terminal or exportable to a file. Built for speed, simplicity, and fun, this project demonstrates multimedia handling, and creative optimization.

## Dependencies

* [stb_image.h](https://github.com/nothings/stb/blob/master/stb_image.h) (already included in src directory)
* [ffmpeg](https://ffmpeg.org/) for extracting invidivual frames from .mp4 videos and retrieving the video resolution

## Features

* Convert static images into ASCII characters, mapped by pixel colors
* Render ASCII images directly in your terminal
* Play videos as ASCII animations
* Export ASCII art to files
  
## Getting Started

### Prerequisites

* GCC or Clang compiler
* Standard C libraries
* Linux/macOS terminal (Windows users: use WSL)
* make

### Installation

Clone the repository:

```bash
git clone https://github.com/aburtasenkov/flower.git
cd flower
```

## ▶️ Usage

Run the program with an image or video file:

```bash
./main <file>
```

### Options

* `-b <size>` : Set block size (controls ASCII resolution, e.g. 30 means one ASCII character is average of a 30x30 pixel block)
* `-f <fps>`  : Frames per second (for videos)
* `-o <file>` : Write ASCII output to a file

The order the these options doesn't matter.

### Examples

Print an image in terminal:

```bash
./main sample.png
```

Using blocksize for scaling:

```bash
./main sample.png -b 30
```

Result:

<img width="460" height="911" alt="image" src="https://github.com/user-attachments/assets/421d7761-583a-456d-8fb3-5c26d7d07b44" />


Save ASCII art to a file:

```bash
./main sample.png -o output.ppm
```

Play a video in ASCII:

```bash
./main video.mp4 -b 8 -f 15
```

## 📈 Roadmap

* [ ] Windows support
* [ ] Color ASCII rendering
* [ ] Interactive playback controls
* [ ] Support for additional codecs

## Constraints

* Video playback currently supports ~125 frames (tested on Ryzen 7).
* Precomputing frames allows extremely high FPS (up to ~6000), but can take a long setup depending on video length.
* The current implementation streams directly from ffmpeg, avoiding performance-killing temp file I/O.
* For better performance, clear your terminal before playing a video.

## Author

Developed by **Anton burtasenkov** – part of my [GitHub Portfolio](https://github.com/aburtasenkov). This project highlights my skills in:

* Low-level programming in C
* Command-line application design
* Modular software architecture
* Performance optimization

## License

This project is licensed under the MIT License – see the [LICENSE](LICENSE) file for details.
