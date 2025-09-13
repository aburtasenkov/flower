# flower - Terminal Multimedia Player

Why is if called flower?

-I wanted to send my girlfriend an ascii flower.

Flower is a C-based tool that converts **images and videos into ASCII**, viewable directly in your terminal or exportable to a file. Built for speed, simplicity, and fun, this project demonstrates multimedia handling, and creative optimization.

## Dependencies

* [stbi_image.h](https://github.com/nothings/stb/blob/master/stb_image.h) (already included in src directory)
* ffmpeg for extracting invidivual frames from .mp4 videos

## Features

* Converting static images to ASCII arrays of characters, depending on colors of corresponding pixels
* Displaying these images in terminal
* Displaying videos in terminal as ACSII images
  
## Getting Started

### Prerequisites

* GCC or Clang compiler
* Standard C libraries
* Linux/macOS terminal (Windows users can run with WSL)

### Installation

Clone the repository:

```bash
git clone https://github.com/aburtasenkov/flower.git
cd flower
```

Build the project:

```bash
bash compile.sh
```

This will generate the `main` executable.

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

## Author

Developed by **Anton burtasenkov** – part of my [GitHub Portfolio](https://github.com/aburtasenkov). This project highlights my skills in:

* Low-level programming with C
* Command-line application design
* Modular software architecture
* Code optimization

## License

This project is licensed under the MIT License – see the [LICENSE](LICENSE) file for details.
