# Terminal Software Rasterizer

Renders 3D models in the terminal. No dependencies needed! 

## Description

This is specifically designed to be as barebones as possible. The thought is that it is made for people to fork this and have it running on crazy devices (a DOOM wannabe, I guess). I wanted to ensure that this didn't require any third-party dependencies, so it could truly run on any machine with a terminal (ironically, I haven't tested this on a Windows machine :P). If you look at the make file, it is as barebones as can be, and the code itself uses somewhat new features (that's my bad; if anyone wants to port this to C99, please be my guest).

## Getting Started

### Dependencies

* C++17
* A terminal (ideally supports ANSI colors for better looks but not required)

### Executing program

* Run `make` (This should create an executable file called "TerminalSoftwareRasterizer")
* Then run `./TerminalSoftwareRasterizer [model path] [zoom] [width (optional)] [height (optional)]`
* This can only display STL files
* Press H for help using the program once running

## Demos

<img width="1462" height="850" alt="Screenshot 2025-09-05 at 11 21 17 PM" src="https://github.com/user-attachments/assets/73700568-2c3a-4291-9c66-6b48cae52022" />
<img width="1462" height="850" alt="Screenshot 2025-09-05 at 11 22 32 PM" src="https://github.com/user-attachments/assets/bf611459-4afb-42cd-b354-a0a35864c694" />
<img width="1462" height="850" alt="Screenshot 2025-09-05 at 11 23 32 PM" src="https://github.com/user-attachments/assets/41885bba-b9fb-45c0-a159-5b27004d318c" />
<img width="1462" height="850" alt="Screenshot 2025-09-05 at 11 26 08 PM" src="https://github.com/user-attachments/assets/e9c55e88-0235-4509-b6f2-7f54ec0f9271" />

## License


This project is licensed under the MIT License - see the LICENSE file for details
