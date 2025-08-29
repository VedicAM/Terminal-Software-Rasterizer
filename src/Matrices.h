#ifndef MATRICES_H
#define MATRICES_H

#include <math.h>

struct mat2x3 {
    float m[2][3];

    mat2x3(float a00, float a01, float a02,
           float a10, float a11, float a12) {
        m[0][0] = a00; m[0][1] = a01; m[0][2] = a02;
        m[1][0] = a10; m[1][1] = a11; m[1][2] = a12;
    }
};

struct vec2 {
    double x, y;

    vec2 operator-(const vec2& other) const {
        return {x - other.x, y - other.y};
    }

    vec2 operator+(const vec2& other) const {
        return {x + other.x, y + other.y};
    }

    vec2 operator*(int scalar) const {
        return {x * scalar, y * scalar};
    }

    friend vec2 operator*(int scalar, const vec2& v) {
        return {v.x * scalar, v.y * scalar};
    }
};

struct vec3 {
    double x, y, z;

    vec3 operator-(const vec3& other) const {
        return {x - other.x, y - other.y, z - other.z};
    }

    vec3 operator+(const vec3& other) const {
        return {x + other.x, y + other.y, z + other.z};
    }

    vec3 operator*(int scalar) const {
        return {x * scalar, y * scalar, z * scalar};
    }

    friend vec3 operator*(int scalar, const vec3& v) {
        return {v.x * scalar, v.y * scalar, v.z * scalar};
    }
};

inline vec3 operator*(const mat2x3& mat, const vec3& v) {
    vec3 res;
    float result[2] = {0};

    for(int i = 0; i < 2; i++) {
        for(int j = 0; j < 3; j++) {
            float val = (j == 0) ? v.x : ((j == 1) ? v.y : v.z);
            result[i] += mat.m[i][j] * val;
        }
    }

    res.x = result[0];
    res.y = result[1];
    res.z = 0;
    return res;
}

inline vec3 cross(const vec3& a, const vec3& b) {
    return { 
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
}

inline float dot(const vec3& a, const vec3& b) {
    return a.x*b.x + a.y*b.y + a.z*b.z;
}

inline vec3 normalize(const vec3& v) {
    float len = sqrt(dot(v,v));
    if (len == 0) return {0,0,0};
    return { v.x/len, v.y/len, v.z/len };
}


#endif