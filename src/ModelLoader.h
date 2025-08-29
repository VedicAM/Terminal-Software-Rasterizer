#include "Matrices.h"

#include <vector>
#include <string>

#ifndef MODELLOADER_H
#define MODELLOADER_H

class ModelLoader{
    private:
        void LoadSTL();
        void LoadSTLBinary();
        void LoadSTLASCII();

    public:
        std::vector<vec3> vertices;
        std::vector<vec3> normals;
        std::string path;

    public:
        ModelLoader(std::string modelPath);
        ~ModelLoader();
};

#endif