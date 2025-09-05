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

        float readFloat(std::ifstream &fileStream);
        vec3 readVector3(std::ifstream &fileStream);

    public:
        std::vector<vec3> vertices;
        std::string path;

    public:
        ModelLoader(std::string modelPath);
        ~ModelLoader();
};

#endif