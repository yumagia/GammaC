#ifndef SCENE_INCLUDED
#define SCENE_INCLUDED

#include "glad/glad.h"
#include <glm/glm.hpp>

#include "GammaFile.hpp"
#include "Camera.hpp"
#include "ApplicationSpecification.hpp"
#include "Shader.hpp"

#include <vector>
#include <iostream>

struct Face {
    GLuint firstIndex, numIndices;
};

struct RenderModel {
    GLuint firstIndex, numIndices;
};

class Scene {
public:
    Scene(ApplicationSpecification *appSpec) : appSpec(appSpec) {}
    ~Scene();

    void OnInitialize();
    void OnRender();
    void OnTerminate();

    void OnMouseControl(float dx, float dy);
    
    void LoadFile(std::string fileName);

private:
    

    void SetupMesh();

    BspFile *bspFile;

    Camera camera;

    Shader *shader;

    ApplicationSpecification *appSpec;

    std::vector<RenderModel> sceneModels;
    std::vector<Face> sceneFaces;
    std::vector<GLuint> sceneIndices;
    std::vector<GLfloat> sceneVerts;

    GLuint vao, vbo, ebo;
    GLuint uniView, uniProj;
};

#endif