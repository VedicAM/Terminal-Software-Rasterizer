#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <unistd.h>
#include <termios.h>
#include <vector>
#include <cmath>
#include <iostream>
#include <algorithm>
#include <thread>
#include <chrono>


#include "Matrices.h"
#include "Display.h"
#include "ModelLoader.h"
#include "IO.h"

enum Colors {
    BLACK = 0,
    RED = 1,
    GREEN = 2,
    YELLOW = 3,
    BLUE = 4,
    MAGENTA = 5,
    CYAN = 6,
    WHITE = 7
};

// =================== Math Helpers ===================

vec3 rotateZ(vec3 point, float angle){
    vec3 returnPoint;
    float rad = angle * M_PI / 180.0f;
    returnPoint.x = cos(rad) * point.x - sin(rad) * point.y;
    returnPoint.y = sin(rad) * point.x + cos(rad) * point.y;
    returnPoint.z = point.z;
    return returnPoint;
}

vec3 rotateX(vec3 point, float angle){
    vec3 returnPoint;
    float rad = angle * M_PI / 180.0f;
    returnPoint.x = point.x;
    returnPoint.y = cos(rad) * point.y - sin(rad) * point.z;
    returnPoint.z = sin(rad) * point.y + cos(rad) * point.z;
    return returnPoint;
}

std::vector<int> generateIndices(const std::vector<vec3>& vertices) {
    std::vector<int> indices;
    indices.reserve(vertices.size());
    for (size_t i = 0; i < vertices.size(); i++) indices.push_back(static_cast<int>(i));
    return indices;
}

vec3 computeCentroid(const std::vector<vec3>& vertices) {
    vec3 c{0,0,0};
    if (vertices.empty()) return c;
    for (auto& v : vertices) { c.x += v.x; c.y += v.y; c.z += v.z; }
    c.x /= vertices.size(); c.y /= vertices.size(); c.z /= vertices.size();
    return c;
}

int colorToGreys(int intensity) {
    if (intensity < 64) return 0;
    else if (intensity < 128) return 8;
    else if (intensity < 192) return 7;
    else return 15;
}

void drawStatusBar(int width, int height, float zoom, float angleX, float angleZ, int triangleCount, int vertexCount) {
    std::cout << "\033[" << height << ";1H";
    std::cout << "\033[44m\033[37m";
    
    char statusText[256];
    snprintf(statusText, sizeof(statusText), 
             " Size: %dx%d | Zoom: %.2f | Angle X: %.1f° | Angle Z: %.1f° | Triangles: %d | Vertices: %d | ESC:Quit R:Reset +/-:Zoom ",
             width, height, zoom, angleX, angleZ, triangleCount, vertexCount);
    
    int textLen = strlen(statusText);
    if (textLen > width) {
        statusText[width] = '\0';
    } else {
        for (int i = textLen; i < width; i++) {
            statusText[i] = ' ';
        }
        statusText[width] = '\0';
    }
    
    std::cout << statusText;
    std::cout << "\033[0m";
    std::cout.flush();
}

void drawInfoPanel(float zoom, float angleX, float angleZ, int triangleCount, int vertexCount) {
    std::cout << "\033[1;1H\033[46m\033[30m";
    
    char line1[20], line2[20], line3[20];
    snprintf(line1, sizeof(line1), " Zoom:%6.2f ", zoom);
    snprintf(line2, sizeof(line2), " X:%3.0f° Z:%3.0f° ", angleX, angleZ);
    snprintf(line3, sizeof(line3), " Tri: %6d ", triangleCount);
    
    std::cout << line1 << "\033[2;1H\033[46m\033[30m" << line2 << "\033[3;1H\033[46m\033[30m" << line3;
    std::cout << "\033[0m";
    std::cout.flush();
}

void drawHelpMenu(Display &display) {
    int w = display.getWidth();
    int h = display.getHeight();
    int menuWidth = 40;
    int menuHeight = 20;
    int startX = (w - menuWidth) / 2;
    int startY = (h - menuHeight) / 2;
    display.drawRectangle({static_cast<double>(startX), static_cast<double>(startY)}, {static_cast<double>(startX+menuWidth), static_cast<double>(startY+menuHeight)}, WHITE);

    std::vector<std::string> lines = {
        "         HELP MENU         ",
        "===========================",
        " ESC       : Quit          ",
        " R         : Reset view    ",
        " +/-       : Zoom in/out   ",
        " Mouse Drag: Rotate model  ",
        " Scroll    : Zoom          ",
        " H         : Toggle help   ",
        "  Press H again to close   "
    };

    int textY = startY + 2;
    for (const auto &line : lines) {
        int textX = startX + (menuWidth - line.size()) / 2;
        std::cout << "\033[" << textY++ << ";" << textX << "H";
        std::cout << "\033[47m\033[30m" << line << "\033[0m";
    }
}


struct Triangle {
    int i0, i1, i2;
    float avgZ;
    vec3 normal;
    float intensity;
};

// =================== Main ===================

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " model.stl zoom width(optional) height(optional)\n";
        return 1;
    }

    enableRawMode();


    Display* display = nullptr;

    if(argc == 5) display = new Display(atoi(argv[3]), atoi(argv[4]));
    else display = new Display(151, 75);

    ModelLoader model(argv[1]);
    Mouse mouse;
    
    std::vector<vec3> points = model.vertices;
    if (points.empty()) {
        std::cerr << "No vertices loaded.\n";
        return 1;
    }

    std::cout << " ===================================================================== \n";
    std::cout << "                  Terminal Software Rasterizer (TSR)                   \n";
    std::cout << " ===================================================================== \n";
    std::cout << " Loading model: " << argv[1] << "           \n";
    std::cout << " Zoom: " << argv[2] << " | Number of Triangles: " << points.size() << "\n";
    std::cout << " Controls: ESC=Quit | R=Reset | H=Help Menu | Right Click=Rotate Model \n";
    std::cout.flush();
    
    std::vector<int> indices = generateIndices(points);
    vec3 centroid = computeCentroid(points);

    vec3 lightDirection = normalize(vec3{0.0f, 0.0f, -1.0f});
    vec3 viewDirection = normalize(vec3{0.0f, 0.0f, -1.0f});
   
    vec2 panOffset = {0.0f, 0.0f};

    float angleX = 0.0f;
    float angleZ = 0.0f;
    float zoom = std::stof(argv[2]);
    float initialZoom = zoom;

    std::vector<vec3> transformed(points.size());
    std::vector<int> projectedX(points.size());
    std::vector<int> projectedY(points.size());

    static int lastX = -1, lastY = -1;
    static int rightDown = 0;

    int centerX = display->getWidth() / 2;
    int centerY = display->getHeight() / 2;

    bool hideUI = false;

    while (true) {
        // ==== Handle input ====
        unsigned char buf[32];
        int n = read(STDIN_FILENO, buf, sizeof(buf));
        
        if (n > 0) {
            mouse.update(buf, n);

            if(mouse.scrollUp || mouse.scrollDown) {
                float zoomFactor = mouse.scrollUp ? 1.1f : 0.9f;
                float mouseScreenX = mouse.x - centerX;
                float mouseScreenY = mouse.y - centerY;
                panOffset.x = mouseScreenX + (panOffset.x - mouseScreenX) * zoomFactor;
                panOffset.y = mouseScreenY + (panOffset.y - mouseScreenY) * zoomFactor;
                zoom *= zoomFactor;
            } 
            else if(mouse.button == 2 && !mouse.motion) { rightDown=1; lastX=mouse.x; lastY=mouse.y; }
            else if(mouse.button == 3) { rightDown=0; }
            else if(mouse.motion && rightDown) {
                if(lastX>=0 && lastY>=0) {
                    angleZ += (mouse.x - lastX) * 5.0f;
                    angleX += (mouse.y - lastY) * 10.0f;
                }
                lastX = mouse.x;
                lastY = mouse.y;
            }

            if(n==1) {
                if (buf[0] == 27) break;

                switch(buf[0]) {
                    case 'h': {
                        hideUI = !hideUI;
                        break;
                    }
                    case '+': case '=': zoom *= 1.1f; break;
                    case '-': case '_': zoom *= 0.9f; break;
                    case 'r': case 'R': {
                        angleX=angleZ=0; 
                        zoom=initialZoom; 
                        panOffset={0,0}; 
                        break;
                    }
                }
            }
        }

        // ==== Rendering ====
        display->clear(CYAN);

        for (size_t i = 0; i < points.size(); ++i) {
            vec3 shifted = { points[i].x - centroid.x,
                             points[i].y - centroid.y,
                             points[i].z - centroid.z };

            vec3 r = rotateZ(shifted, angleZ);
            r = rotateX(r, angleX);

            transformed[i] = r;

            vec2 proj = vec2{1.0f/0.35f * r.x, 1.0f/0.35f * r.y};
            projectedX[i] = static_cast<int>(proj.x * zoom + centerX + panOffset.x);
            projectedY[i] = static_cast<int>(proj.y * zoom + centerY + panOffset.y);
        }

        std::vector<Triangle> visibleTriangles;

        for (size_t i = 0; i + 2 < indices.size(); i += 3) {
            int i0 = indices[i], i1 = indices[i+1], i2 = indices[i+2];

            if (i0 < 0 || i1 < 0 || i2 < 0 ||
                (size_t)i0 >= transformed.size() ||
                (size_t)i1 >= transformed.size() ||
                (size_t)i2 >= transformed.size()) continue;

            vec3 v0 = transformed[i0];
            vec3 v1 = transformed[i1];
            vec3 v2 = transformed[i2];

            vec3 e1 = {v1.x - v0.x, v1.y - v0.y, v1.z - v0.z};
            vec3 e2 = {v2.x - v0.x, v2.y - v0.y, v2.z - v0.z};
            vec3 n = normalize(cross(e1, e2));

            float backfaceDot = dot(n, viewDirection);
            if (backfaceDot < 0.0f) {
                continue;
            }

            float intensity = dot(n, lightDirection);
            if (intensity > 0.0f) {
                float avgZ = (v0.z + v1.z + v2.z) / 3.0f;
                
                Triangle tri;
                tri.i0 = i0;
                tri.i1 = i1;
                tri.i2 = i2;
                tri.avgZ = avgZ;
                tri.normal = n;
                tri.intensity = intensity;
                
                visibleTriangles.push_back(tri);
            }
        }

        std::sort(visibleTriangles.begin(), visibleTriangles.end(), 
                  [](const Triangle& a, const Triangle& b) {
                      return a.avgZ > b.avgZ;
                  });

        for (const Triangle& tri : visibleTriangles) {
            int grey = static_cast<int>(std::round(tri.intensity * 255.0f));
            int greyShade = colorToGreys(grey);

            vec2 A = { static_cast<double>(projectedX[tri.i0]),
                       static_cast<double>(projectedY[tri.i0]) };
            vec2 B = { static_cast<double>(projectedX[tri.i1]),
                       static_cast<double>(projectedY[tri.i1]) };
            vec2 C = { static_cast<double>(projectedX[tri.i2]),
                       static_cast<double>(projectedY[tri.i2]) };

            display->drawTriangle(A, B, C, static_cast<Colors>(greyShade));
        }
        
        display->render();
        
        if (!hideUI) {
            drawInfoPanel(zoom, angleX, angleZ, visibleTriangles.size(), points.size());
            drawStatusBar(display->getWidth(), display->getHeight(), zoom, angleX, angleZ,
                        visibleTriangles.size(), points.size());
        } else {
            drawHelpMenu(*display);
        }


        std::cout.flush();

        
        std::this_thread::sleep_for(std::chrono::milliseconds(17));
    }

    disableRawMode();
    
    std::cout << "\033[2J\033[?25h\033[0m" << "\n";
    delete display;
    return 0;
}