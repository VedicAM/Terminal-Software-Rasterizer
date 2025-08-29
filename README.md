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

## Demos

<img width="1470" height="858" alt="Screenshot 2025-08-29 at 4 10 52 PM" src="https://github.com/user-attachments/assets/b8a91ded-f436-481b-9720-3bbad9117d72" />
<img width="1470" height="858" alt="Screenshot 2025-08-29 at 4 11 22 PM" src="https://github.com/user-attachments/assets/90c4b1c5-a1b5-404c-a4ab-761281fb6f58" />
<img width="1470" height="858" alt="Screenshot 2025-08-29 at 4 18 06 PM" src="https://github.com/user-attachments/assets/84bf78cc-8fbe-438f-8666-59601e7bca59" />
<img width="1470" height="858" alt="Screenshot 2025-08-29 at 4 14 33 PM" src="https://github.com/user-attachments/assets/71c571c8-3e3b-4797-9a2d-dfed729d95be" />

## License

This project is licensed under the MIT License - see the LICENSE file for details
