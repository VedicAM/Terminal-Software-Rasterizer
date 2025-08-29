# Terminal Software Rasterizer

Renders 3D models in the terminal. No dependencies needed! 

## Description

This is specifically designed to be as barebones as possible. The thought is that it is made for people to fork this and have it running on crazy devices (a DOOM wannabe, I guess). I wanted to ensure that this didn't require any third-party dependencies, so it could truly run on any machine with a terminal (ironically, I haven't tested this on a Windows machine :P). If you look at the make file, it is as barebones as can be, and the code itself uses somewhat new features (that's my bad; if anyone wants to port this to C99, please be my guest).

## Getting Started

### Dependencies

* C++17
* A terminal (ideally supports ANSI colors for better looks but not required)

### Executing program

* Run `make` (This should create an executable file called main)
* Then run `./main [model path] [zoom] [width (optional)] [height (optional)]`
* This can only display STL files

## License

This project is licensed under the MIT License - see the LICENSE file for details
