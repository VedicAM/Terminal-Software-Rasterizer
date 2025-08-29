#include "Display.h"

#include <iostream>

Display::Display(int w, int h) : width(w), height(h) {
    if (height % 2 != 0) height++;
    frontBuffer.resize(height, std::vector<int>(width, 0));
    backBuffer.resize(height, std::vector<int>(width, 0));
    std::cout << "\033[2J\033[?25l";
}

Display::~Display() {
    std::cout << "\033[?25h\033[0m";
}

/* PUBLIC FUNCTIONS*/
void Display::setPixel(int x, int y, int color) {
      if (x >= 0 && x < width && y >= 0 && y < height) {
        backBuffer[y][x] = color;
    }
}

void Display::clear(int color) {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            backBuffer[y][x] = color;
        }
    }
}

void Display::render() {
    swapBuffers();

    std::cout << "\033[H";
    for (int y = 0; y < height; y += 2) {
        for (int x = 0; x < width; x++) {
            int topColor = frontBuffer[y][x];
            int bottomColor = (y + 1 < height) ? frontBuffer[y + 1][x] : 0;

            if (topColor == bottomColor) std::cout << getColorCode(topColor) << " ";
            else std::cout << "\033[38;5;" << topColor << "m\033[48;5;" << bottomColor << "m▀";
        }
        std::cout << "\033[0m\n";
    }
    std::cout.flush();
}

/* SHAPES */
void Display::drawLine(vec2 a, vec2 b, int color) {
    int dx = abs(b.x - a.x);
    int dy = abs(b.y - a.y);
    int sx = (a.x < b.x) ? 1 : -1;
    int sy = (a.y < b.y) ? 1 : -1;
    int err = dx - dy;
    
    int x = a.x, y = a.y;
    while (true) {
        setPixel(x, y, color);
        if (x == b.x && y == b.y) break;
        
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x += sx;
        }
        if (e2 < dx) {
            err += dx;
            y += sy;
        }
    }
}

void Display::drawWireTriangle(vec2 a, vec2 b, vec2 c, int color) {
    drawLine(a, b, color);
    drawLine(b, c, color);
    drawLine(c, a, color);
}

void Display::drawTriangle(vec2 a, vec2 b, vec2 c, int color) {
    // 28.4 fixed-point
    int X1 = iround(a.x * 16.0f);
    int Y1 = iround(a.y * 16.0f);
    int X2 = iround(b.x * 16.0f);
    int Y2 = iround(b.y * 16.0f);
    int X3 = iround(c.x * 16.0f);
    int Y3 = iround(c.y * 16.0f);

    int DX12 = X1 - X2, DY12 = Y1 - Y2;
    int DX23 = X2 - X3, DY23 = Y2 - Y3;
    int DX31 = X3 - X1, DY31 = Y3 - Y1;

    int FDX12 = DX12 << 4, FDY12 = DY12 << 4;
    int FDX23 = DX23 << 4, FDY23 = DY23 << 4;
    int FDX31 = DX31 << 4, FDY31 = DY31 << 4;

    int minx = (std::min({X1,X2,X3}) + 0xF) >> 4;
    int maxx = (std::max({X1,X2,X3}) + 0xF) >> 4;
    int miny = (std::min({Y1,Y2,Y3}) + 0xF) >> 4;
    int maxy = (std::max({Y1,Y2,Y3}) + 0xF) >> 4;

    const int q = 8; // block size
    minx &= ~(q - 1);
    miny &= ~(q - 1);

    // Half-edge constants
    int C1 = DY12*X1 - DX12*Y1; if(DY12<0 || (DY12==0 && DX12>0)) C1++;
    int C2 = DY23*X2 - DX23*Y2; if(DY23<0 || (DY23==0 && DX23>0)) C2++;
    int C3 = DY31*X3 - DX31*Y3; if(DY31<0 || (DY31==0 && DX31>0)) C3++;

    for(int y = miny; y < maxy; y += q) {
        for(int x = minx; x < maxx; x += q) {
            int x0 = x << 4, y0 = y << 4;
            int x1b = (x+q-1)<<4, y1b = (y+q-1)<<4;

            // Evaluate corners
            int a_mask = ((C1 + DX12*y0 - DY12*x0 >0) <<0) |
                         ((C1 + DX12*y0 - DY12*x1b>0) <<1) |
                         ((C1 + DX12*y1b - DY12*x0>0) <<2) |
                         ((C1 + DX12*y1b - DY12*x1b>0) <<3);

            int b_mask = ((C2 + DX23*y0 - DY23*x0 >0) <<0) |
                         ((C2 + DX23*y0 - DY23*x1b>0) <<1) |
                         ((C2 + DX23*y1b - DY23*x0>0) <<2) |
                         ((C2 + DX23*y1b - DY23*x1b>0) <<3);

            int c_mask = ((C3 + DX31*y0 - DY31*x0 >0) <<0) |
                         ((C3 + DX31*y0 - DY31*x1b>0) <<1) |
                         ((C3 + DX31*y1b - DY31*x0>0) <<2) |
                         ((C3 + DX31*y1b - DY31*x1b>0) <<3);

            if(a_mask==0 || b_mask==0 || c_mask==0) continue; // skip block

            // Draw each pixel in block
            int CY1 = C1 + DX12*y0 - DY12*x0;
            int CY2 = C2 + DX23*y0 - DY23*x0;
            int CY3 = C3 + DX31*y0 - DY31*x0;

            for(int iy=y; iy<y+q; iy++) {
                int CX1 = CY1, CX2 = CY2, CX3 = CY3;
                for(int ix=x; ix<x+q; ix++) {
                    if(CX1>0 && CX2>0 && CX3>0) setPixel(ix, iy, color);
                    CX1 -= FDY12; CX2 -= FDY23; CX3 -= FDY31;
                }
                CY1 += FDX12; CY2 += FDX23; CY3 += FDX31;
            }
        }
    }
}


/* PRIVATE FUNCTIONS */
std::string Display::getColorCode(int color) {
    std::stringstream ss;
    ss << "\033[48;5;" << color << "m";
    return ss.str();
}

int Display::iround(float x) {
    return static_cast<int>(x + 0.5f);
}

void Display::swapBuffers() {
    std::swap(frontBuffer, backBuffer);
}