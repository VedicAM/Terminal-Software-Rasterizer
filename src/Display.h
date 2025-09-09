#ifndef DISPLAY_H
#define DISPLAY_H

#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <cstring>

#include "Matrices.h"

class Display {
private:
    int width, height;

    std::vector< std::vector<int> > frontBuffer;
    std::vector< std::vector<int> > backBuffer;
    
private:
    std::string getColorCode(int color);
    int iround(float x);
    void swapBuffers();

    
public:
    Display(int w, int h);
    ~Display();
    
    void setPixel(int x, int y, int color);
    void clear(int color = 0);
    void render();

    /* SHAPES */
    void drawLine(vec2 a, vec2 b, int color);
    void drawWireTriangle(vec2 a, vec2 b, vec2 c, int color);
    void drawTriangle(vec2 a, vec2 b, vec2 c, int color);
    void drawRectangle(vec2 a, vec2 b, int color);
    
    int getWidth() const { return width; }
    int getHeight() const { return height; }
};

#endif
