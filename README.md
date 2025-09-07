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
* Then run `./TerminalSoftwareRasterizer [model path] [zoom (optional)] [width (optional)] [height (optional)]`
* This can only display STL files
* Press H for help using the program once running

## Demos

<img width="1462" height="850" alt="Screenshot 2025-09-07 at 12 07 12 AM" src="https://github.com/user-attachments/assets/7b6acb37-7d5e-4e56-b366-53fe1bcfa750" />
<img width="1462" height="850" alt="Screenshot 2025-09-07 at 12 08 00 AM" src="https://github.com/user-attachments/assets/098f4214-cdd1-438d-8438-aa5942ec6883" />
<img width="1462" height="850" alt="Screenshot 2025-09-07 at 12 08 59 AM" src="https://github.com/user-attachments/assets/1826b1d4-dfc8-49ba-bcbf-a9e8bb6795a0" />
<img width="1462" height="850" alt="Screenshot 2025-09-07 at 12 10 17 AM" src="https://github.com/user-attachments/assets/075983fa-1fc4-4f46-95e2-408f1b37750d" />

## License


This project is licensed under the MIT License - see the LICENSE file for details
