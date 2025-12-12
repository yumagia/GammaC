#ifndef SCENE_INCLUDED
#define SCENE_INCLUDED

#include "glad/glad.h"
#include <SDL2/SDL.h>
#include <glm/glm.hpp>

#include "GammaFile.hpp"

#include <vector>
#include <iostream>

struct Face {
    unsigned int firstIndex, numIndices;
};

struct Vertex {
    glm::vec3 position;
};

class Scene {
public:
    Scene() {}

    void OnInitialize();
    
    void LoadFile(std::string fileName);

private:
    

    void SetupMesh();

    BspFile *bspFile;

    std::vector<Face> faces;
    std::vector<GLuint> indices;
    std::vector<Vertex> vertices;

    GLuint VAO, VBO, EBO;
};

#endif