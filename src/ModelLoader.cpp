#include "ModelLoader.h"


#include <iostream>
#include <vector>
#include <fstream> 
#include <sstream>
#include <string>

ModelLoader::ModelLoader(std::string modelPath) : path(modelPath){
    std::filesystem::path modelEx(path.c_str());
    if(modelEx.extension() == ".stl") LoadSTL();
}

ModelLoader::~ModelLoader(){

}

void ModelLoader::LoadSTLASCII(){
    std::vector<unsigned int> vertexIndices, normalIndices;
    std::vector<vec3> tempVertices;
    std::vector<vec3> tempNormals;

    std::ifstream fileStream(path.c_str());

    if(!fileStream.is_open()) fileStream.close();

    for (std::string line; std::getline(fileStream, line);){
        std::istringstream in(line);

        std::string type;
        in >> type;

        if (type == "facet") {
            std::string normalKeyword;
            in >> normalKeyword;
            if (normalKeyword == "normal") {
                vec3 normal;
                in >> normal.x >> normal.y >> normal.z;
                tempNormals.push_back(normal);
                tempNormals.push_back(normal);
                tempNormals.push_back(normal);
            }
        }else if(type == "vertex"){
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
    normals = tempNormals;

    fileStream.close();
}

void ModelLoader::LoadSTLBinary(){
    // REAL32 is just a float :P
    
    std::vector<unsigned int> vertexIndices, normalIndices;
    std::vector<vec3> tempVertices;
    std::vector<vec3> tempNormals;

    std::ifstream fileStream(path.c_str(), std::ios::binary);

    if(!fileStream.is_open()){
        std::cerr << "Error opening " << path.c_str() << std::endl;
        fileStream.close();
        exit(-1);
    }

    std::vector<char> header(80);
    fileStream.read(header.data(), header.size());

    std::vector<uint8_t> rawTrianglesNumber(4);
    fileStream.read(reinterpret_cast<char *>(rawTrianglesNumber.data()), rawTrianglesNumber.size());

    uint32_t triangleNumber = (rawTrianglesNumber[3] << 24) | (rawTrianglesNumber[2] << 16) | (rawTrianglesNumber[1] << 8) | rawTrianglesNumber[0];

    fileStream.seekg(0x86, std::ios::beg);

    for(int i = 0; i < triangleNumber * 50; i += 50){
        vec3 normal;

        std::vector<uint8_t> rawNormalX(4);
        fileStream.read(reinterpret_cast<char *>(rawNormalX.data()), rawNormalX.size());
        uint32_t littleNormalX = (static_cast<uint32_t>(rawNormalX[3]) << 24) |
                        (static_cast<uint32_t>(rawNormalX[2]) << 16) |
                        (static_cast<uint32_t>(rawNormalX[1]) << 8)  |
                            static_cast<uint32_t>(rawNormalX[0]);
        
        float normalX = *((float*)&littleNormalX);

        std::vector<uint8_t> rawNormalY(4);
        fileStream.read(reinterpret_cast<char *>(rawNormalY.data()), rawNormalY.size());
        uint32_t littleNormalY = (static_cast<uint32_t>(rawNormalY[3]) << 24) |
                        (static_cast<uint32_t>(rawNormalY[2]) << 16) |
                        (static_cast<uint32_t>(rawNormalY[1]) << 8)  |
                            static_cast<uint32_t>(rawNormalY[0]);
        
        float normalY = *((float*)&littleNormalY);

        std::vector<uint8_t> rawNormalZ(4);
        fileStream.read(reinterpret_cast<char *>(rawNormalZ.data()), rawNormalZ.size());
        uint32_t littleNormalZ = (static_cast<uint32_t>(rawNormalZ[3]) << 24) |
                        (static_cast<uint32_t>(rawNormalZ[2]) << 16) |
                        (static_cast<uint32_t>(rawNormalZ[1]) << 8)  |
                            static_cast<uint32_t>(rawNormalZ[0]);
        
        float normalZ = *((float*)&littleNormalZ);

        normal.x = normalX;
        normal.y = normalY;
        normal.z = normalZ;

        tempNormals.push_back(normal);
        tempNormals.push_back(normal);
        tempNormals.push_back(normal);


        vec3 vector1;

        std::vector<uint8_t> rawVectorX1(4);
        fileStream.read(reinterpret_cast<char *>(rawVectorX1.data()), rawVectorX1.size());
        uint32_t littleVectorX1 = (static_cast<uint32_t>(rawVectorX1[3]) << 24) |
                        (static_cast<uint32_t>(rawVectorX1[2]) << 16) |
                        (static_cast<uint32_t>(rawVectorX1[1]) << 8)  |
                            static_cast<uint32_t>(rawVectorX1[0]);
        
        float vectorX1 = *((float*)&littleVectorX1);

        std::vector<uint8_t> rawVectorY1(4);
        fileStream.read(reinterpret_cast<char *>(rawVectorY1.data()), rawVectorY1.size());
        uint32_t littleVectorY1 = (static_cast<uint32_t>(rawVectorY1[3]) << 24) |
                        (static_cast<uint32_t>(rawVectorY1[2]) << 16) |
                        (static_cast<uint32_t>(rawVectorY1[1]) << 8)  |
                            static_cast<uint32_t>(rawVectorY1[0]);
        
        float vectorY1 = *((float*)&littleVectorY1);

        std::vector<uint8_t> rawVectorZ1(4);
        fileStream.read(reinterpret_cast<char *>(rawVectorZ1.data()), rawVectorZ1.size());
        uint32_t littleVectorZ1 = (static_cast<uint32_t>(rawVectorZ1[3]) << 24) |
                        (static_cast<uint32_t>(rawVectorZ1[2]) << 16) |
                        (static_cast<uint32_t>(rawVectorZ1[1]) << 8)  |
                            static_cast<uint32_t>(rawVectorZ1[0]);
        
        float vectorZ1 = *((float*)&littleVectorZ1);

        vector1.x = vectorX1;
        vector1.y = vectorY1;
        vector1.z = vectorZ1;

        tempVertices.push_back(vector1);


        vec3 vector2;

        std::vector<uint8_t> rawVectorX2(4);
        fileStream.read(reinterpret_cast<char *>(rawVectorX2.data()), rawVectorX2.size());
        uint32_t littleVectorX2 = (static_cast<uint32_t>(rawVectorX2[3]) << 24) |
                        (static_cast<uint32_t>(rawVectorX2[2]) << 16) |
                        (static_cast<uint32_t>(rawVectorX2[1]) << 8)  |
                            static_cast<uint32_t>(rawVectorX2[0]);
        
        float vectorX2 = *((float*)&littleVectorX2);

        std::vector<uint8_t> rawVectorY2(4);
        fileStream.read(reinterpret_cast<char *>(rawVectorY2.data()), rawVectorY2.size());
        uint32_t littleVectorY2 = (static_cast<uint32_t>(rawVectorY2[3]) << 24) |
                        (static_cast<uint32_t>(rawVectorY2[2]) << 16) |
                        (static_cast<uint32_t>(rawVectorY2[1]) << 8)  |
                            static_cast<uint32_t>(rawVectorY2[0]);
        
        float vectorY2 = *((float*)&littleVectorY2);

        std::vector<uint8_t> rawVectorZ2(4);
        fileStream.read(reinterpret_cast<char *>(rawVectorZ2.data()), rawVectorZ2.size());
        uint32_t littleVectorZ2 = (static_cast<uint32_t>(rawVectorZ2[3]) << 24) |
                        (static_cast<uint32_t>(rawVectorZ2[2]) << 16) |
                        (static_cast<uint32_t>(rawVectorZ2[1]) << 8)  |
                            static_cast<uint32_t>(rawVectorZ2[0]);
        
        float vectorZ2 = *((float*)&littleVectorZ2);

        vector2.x = vectorX2;
        vector2.y = vectorY2;
        vector2.z = vectorZ2;

        tempVertices.push_back(vector2);


        vec3 vector3;

        std::vector<uint8_t> rawVectorX3(4);
        fileStream.read(reinterpret_cast<char *>(rawVectorX3.data()), rawVectorX3.size());
        uint32_t littleVectorX3 = (static_cast<uint32_t>(rawVectorX3[3]) << 24) |
                        (static_cast<uint32_t>(rawVectorX3[2]) << 16) |
                        (static_cast<uint32_t>(rawVectorX3[1]) << 8)  |
                            static_cast<uint32_t>(rawVectorX3[0]);
        
        float vectorX3 = *((float*)&littleVectorX3);

        std::vector<uint8_t> rawVectorY3(4);
        fileStream.read(reinterpret_cast<char *>(rawVectorY3.data()), rawVectorY3.size());
        uint32_t littleVectorY3 = (static_cast<uint32_t>(rawVectorY3[3]) << 24) |
                        (static_cast<uint32_t>(rawVectorY3[2]) << 16) |
                        (static_cast<uint32_t>(rawVectorY3[1]) << 8)  |
                            static_cast<uint32_t>(rawVectorY3[0]);
        
        float vectorY3 = *((float*)&littleVectorY3);

        std::vector<uint8_t> rawVectorZ3(4);
        fileStream.read(reinterpret_cast<char *>(rawVectorZ3.data()), rawVectorZ3.size());
        uint32_t littleVectorZ3 = (static_cast<uint32_t>(rawVectorZ3[3]) << 24) |
                        (static_cast<uint32_t>(rawVectorZ3[2]) << 16) |
                        (static_cast<uint32_t>(rawVectorZ3[1]) << 8)  |
                            static_cast<uint32_t>(rawVectorZ3[0]);
        
        float vectorZ3 = *((float*)&littleVectorZ3);

        vector3.x = vectorX3;
        vector3.y = vectorY3;
        vector3.z = vectorZ3;

        tempVertices.push_back(vector3);

        fileStream.seekg(2, std::ios::cur);
    }

    if (tempVertices.size() % 3 != 0) {
        std::cerr << "Error: STL file vertex count is not a multiple of 3!" << std::endl;
        exit(-1);
    }

    vertices = tempVertices;
    normals = tempNormals;

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