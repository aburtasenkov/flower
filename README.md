# flower

Why is if called flower?

-I wanted to send my girlfriend an ascii flower.

Flower is a C-based tool that converts **images and videos into ASCII art**, viewable directly in your terminal or exportable to a file. Built for speed, simplicity, and fun, this project demonstrates system-level programming, multimedia handling, and creative visualization.

## 🚀 Features

* **Image to ASCII** – Render static images as ASCII art
* **Video to ASCII** – Play videos as ASCII animations in your terminal
* **Output to File** – Save ASCII art
* **Configurable** – Adjust block size, FPS, and output
* **Modular Codebase** – Organized source files for clarity and maintainability

## 🛠️ Tech Stack

* **Language:** C (ANSI C)
* **Tools:** GCC/Clang, Bash (for build)
* **Concepts:** File I/O, command-line argument parsing, multimedia processing, terminal rendering

## 📦 Getting Started

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

* `-b <size>` : Set block size (controls ASCII resolution)
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

Save ASCII art to a file:

```bash
./main sample.png -o output.ppm
```

Play a video in ASCII:

```bash
./main video.mp4 -b 8 -f 15
```

## 📂 Project Structure

```
├── compile.sh      # Build script
├── main.c          # Program entry point
├── src/            # Core source code
│   ├── imageToAscii.c
│   ├── videoToAscii.c
│   ├── Config.h
│   ├── ...
├── todo            # Development notes
└── README.md       # Project documentation
```

## 📈 Roadmap

* [ ] Windows support
* [ ] Color ASCII rendering
* [ ] Interactive playback controls
* [ ] Support for additional codecs

## 🧑‍💻 Author

Developed by **Anton burtasenkov** – part of my [GitHub Portfolio](https://github.com/aburtasenkov). This project highlights my skills in:

* Low-level programming with C
* Command-line application design
* Multimedia processing and visualization
* Modular software architecture

## 📜 License

This project is licensed under the MIT License – see the [LICENSE](LICENSE) file for details.
