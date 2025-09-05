#include <__config>
#include <ostream>
#include <vector>
#include <thread>
#include <chrono>
#include <string>
#include <cmath>
#include <iostream>
#include <algorithm>

#include "Matrices.h"
#include "Display.h"
#include "ModelLoader.h"

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

struct Triangle {
    int i0, i1, i2;
    float avgZ;
    vec3 normal;
    float intensity;
};

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " model.stl zoom width(optional) height(optional)\n";
        return 1;
    }

    Display* display = nullptr;

    if(argc == 5) display = new Display(atoi(argv[3]), atoi(argv[4]));
    else display = new Display(151, 75);

    ModelLoader model(argv[1]);
    
    std::vector<vec3> points = model.vertices;
    if (points.empty()) {
        std::cerr << "No vertices loaded.\n";
        return 1;
    }
    
    std::vector<int> indices = generateIndices(points);
    vec3 centroid = computeCentroid(points);

    mat2x3 projection(
        1.0f/0.35f, 0, 0,
        0, 1.0f/0.35f, 0
    );

    vec3 lightDirection = normalize(vec3{0.0f, 0.0f, -1.0f});
    vec3 viewDirection = normalize(vec3{0.0f, 0.0f, -1.0f});
    float angle = 0.0f;
    float zoom = std::stof(argv[2]);

    std::vector<vec3> transformed(points.size());
    std::vector<int> projectedX(points.size());
    std::vector<int> projectedY(points.size());

    std::cout << "Terminal Software Rasterizer\n";
    std::cout << "Display size: " << display->getWidth() << "x" << display->getHeight() << "\n";
    std::cout << "Zoom: " << argv[2] << "\n";
    std::cout << "Press any key to start demo..." << std::endl;
    std::cin.get();

    while (true) {
        display->clear(CYAN);

        int centerX = display->getWidth() / 2;
        int centerY = display->getHeight() / 2;

        for (size_t i = 0; i < points.size(); ++i) {
            vec3 shifted = { points[i].x - centroid.x,
                             points[i].y - centroid.y,
                             points[i].z - centroid.z };

            vec3 r = rotateZ(shifted, angle);
            r = rotateX(r, angle);

            transformed[i] = r;

            vec2 proj = vec2{1.0f/0.35f * r.x, 1.0f/0.35f * r.y};
            projectedX[i] = static_cast<int>(proj.x * zoom + centerX);
            projectedY[i] = static_cast<int>(proj.y * zoom + centerY);
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

        angle += 2.0f;
        if (angle >= 360.0f) angle -= 360.0f;
        std::this_thread::sleep_for(std::chrono::milliseconds(17));
    }

    delete display;

    return 0;
}