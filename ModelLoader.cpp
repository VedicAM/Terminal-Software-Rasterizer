#include "ModelLoader.h"
#include "Matrices.h"

#include <iostream>
#include <vector>
#include <fstream> 
#include <sstream>
#include <string>
#include <filesystem>

ModelLoader::ModelLoader(std::string modelPath) : path(modelPath){
    std::filesystem::path modelEx(path.c_str());
    if(modelEx.extension() == ".stl") LoadSTL();
}

ModelLoader::~ModelLoader(){

}

void ModelLoader::LoadSTLASCII(){
    std::vector<unsigned int> vertexIndices;
    std::vector<vec3> tempVertices;

    std::ifstream fileStream(path.c_str());

    if(!fileStream.is_open()) fileStream.close();

    for (std::string line; std::getline(fileStream, line);){
        std::istringstream in(line);

        std::string type;
        in >> type;

        if(type == "vertex") {
            vec3 vertex;
            in >> vertex.x >> vertex.y >> vertex.z;
            tempVertices.push_back(vertex);
        }
    }

    if (tempVertices.size() % 3 != 0) {
        std::cerr << "Error: STL file vertex count is not a multiple of 3!" << std::endl;
        exit(-1);
    }

    vertices = tempVertices;
    fileStream.close();
}

float ModelLoader::readFloat(std::ifstream &fileStream) {
    std::vector<uint8_t> rawFloat(4);
    fileStream.read(reinterpret_cast<char *>(rawFloat.data()), rawFloat.size());
    uint32_t littleFloat = (static_cast<uint32_t>(rawFloat[3]) << 24) |
                    (static_cast<uint32_t>(rawFloat[2]) << 16) |
                    (static_cast<uint32_t>(rawFloat[1]) << 8)  |
                        static_cast<uint32_t>(rawFloat[0]); 
    return *((float*)&littleFloat);
}

vec3 ModelLoader::readVector3(std::ifstream &fileStream) {
    vec3 res;
    res.x = readFloat(fileStream);
    res.y = readFloat(fileStream);
    res.z = readFloat(fileStream);

    return res;
}

void ModelLoader::LoadSTLBinary() {
    // REAL32 is just a float :P
    
    std::vector<unsigned int> vertexIndices;
    std::vector<vec3> tempVertices;

    std::ifstream fileStream(path.c_str(), std::ios::binary);

    if(!fileStream.is_open()) {
        std::cerr << "Error opening " << path.c_str() << std::endl;
        fileStream.close();
        exit(-1);
    }

    std::vector<char> header(80);
    fileStream.read(header.data(), header.size());

    std::vector<uint8_t> rawTrianglesNumber(4);
    fileStream.read(reinterpret_cast<char *>(rawTrianglesNumber.data()), rawTrianglesNumber.size());

    uint32_t triangleNumber = (rawTrianglesNumber[3] << 24) | (rawTrianglesNumber[2] << 16) | (rawTrianglesNumber[1] << 8) | rawTrianglesNumber[0];

    fileStream.seekg(84, std::ios::beg);

    for(int i = 0; i < triangleNumber; i++){
        vec3 normal = readVector3(fileStream); //Not used

        vec3 vector1 = readVector3(fileStream);
        tempVertices.push_back(vector1);

        vec3 vector2 = readVector3(fileStream);
        tempVertices.push_back(vector2);

        vec3 vector3 = readVector3(fileStream);
        tempVertices.push_back(vector3);

        fileStream.seekg(2, std::ios::cur);
    }

    if (tempVertices.size() % 3 != 0) {
        std::cerr << "Error: STL file vertex count is not a multiple of 3!" << std::endl;
        exit(-1);
    }

    vertices = tempVertices;
    fileStream.close();
}

void ModelLoader::LoadSTL(){
    std::ifstream fileStream(path.c_str());

    if(!fileStream.is_open()){
        std::cerr << "Error opening " << path.c_str() << std::endl;
        fileStream.close();
        exit(-1);
    }

    std::string firstLine;
    if(std::getline(fileStream, firstLine)){
        std::istringstream in(firstLine);

        std::string type;
        in >> type;

        if(type == "solid"){
            fileStream.close();
            LoadSTLASCII();
        }else{
            fileStream.close();
            LoadSTLBinary();
        }
    }

    fileStream.close();
}