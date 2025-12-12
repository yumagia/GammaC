#include "glad/glad.h"
#include <SDL2/SDL.h>

#include "Scene.hpp"
#include "GammaFile.hpp"
#include "FileReader.hpp"

#include <iostream>

void Scene::OnInitialize() {

}

void Scene::LoadFile(std::string fileName) {
	FileReader loader;

	bspFile = loader.ReadFile(fileName);

	if(!(bspFile && bspFile->valid)) {
		std::cerr << "Error reading BSP file" << std::endl;
        bspFile = NULL;
		return;
	}

    SetupMesh();
}

void Scene::SetupMesh() {
    int numFaces = bspFile->fileHeader.lumps[LUMP_FACES].length;
    //for()

    // Create the OpenGL render data
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint),
        &indices[0], GL_STATIC_DRAW);

    // Vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);  

    glBindVertexArray(0);
}