#ifndef MOUSE_H
#define MOUSE_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>

static struct termios origTermios;

void disableRawMode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &origTermios);
    printf("\033[?1003l\033[?1002l\033[?1000l");
    printf("\033[?25h\033[0m");
    fflush(stdout);
}

void enableRawMode() {
    tcgetattr(STDIN_FILENO, &origTermios);
    atexit(disableRawMode);

    struct termios raw = origTermios;
    raw.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);

    printf("\033[?1000h\033[?1002h\033[?1003h");
    fflush(stdout);
}

struct Mouse {
    int x = 0;
    int y = 0;
    int button = 0;
    bool motion = false;
    bool scrollUp = false;
    bool scrollDown = false;

    void update(unsigned char buf[32], int n) {
        scrollUp = scrollDown = false;

        if (n >= 6 && buf[0] == '\033' && buf[1] == '[' && buf[2] == 'M') {
            int cb = buf[3] - 32;
            x = buf[4] - 32;
            y = buf[5] - 32;

            button = cb & 3;
            motion = (cb & 32) != 0;

            if (cb == 64) scrollUp = true;
            if (cb == 65) scrollDown = true;
        }
    }
};

#endif