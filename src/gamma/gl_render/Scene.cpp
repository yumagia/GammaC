#include "glad/glad.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Scene.hpp"
#include "GammaFile.hpp"
#include "FileReader.hpp"
#include "Camera.hpp"
#include "MathDefs.hpp"

#include <iostream>

Scene::~Scene() {
	delete bspFile;
	delete shader;
}

void Scene::LoadFile(std::string fileName) {
	std::cout << "--- LoadFile ---" << std::endl;

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
	std::cout << "Setting up rendering data... " << std::endl;


	int numVerts = bspFile->fileHeader.lumps[LUMP_VERTS].length;
	sceneVerts.reserve(numVerts * 3);
	for(int i = 0; i < numVerts; i++) {
		FileVert *bspVert = &bspFile->fileVerts[i];

		sceneVerts.push_back(bspVert->point[0]);
		sceneVerts.push_back(bspVert->point[1]);
		sceneVerts.push_back(bspVert->point[2]);
	}

	for(int modelIdx = 0; modelIdx < bspFile->fileHeader.lumps[LUMP_MODELS].length; modelIdx++) {
		RenderModel newModel;
		newModel.firstIndex = sceneIndices.size();
		
		FileModel *bspModel = &bspFile->fileModels[modelIdx];
		int firstFace = bspModel->firstFace;
		int numFaces = bspModel->numFaces;
		for(int i = 0; i < numFaces; i++) {
			Face newFace;

			newFace.firstIndex = sceneIndices.size();

			FileFace *bspFace = &bspFile->fileFaces[i + firstFace];
			int numTris = bspFace->numVerts - 2;
			if(numTris < 1) {
				continue;
			}
			for(int j = 1; j < numTris; j++) {
				sceneIndices.push_back(bspFace->firstVert);
				sceneIndices.push_back(bspFace->firstVert + j);
				sceneIndices.push_back(bspFace->firstVert + j + 1);
			}

			newFace.numIndices = sceneIndices.size();

			sceneFaces.push_back(newFace);
		}

		newModel.numIndices = ((GLuint) sceneIndices.size()) - newModel.firstIndex;
		sceneModels.push_back(newModel);
	}

	std::cout << "	Number of render indices: " << sceneIndices.size() << std::endl;
	std::cout << "	Number of render verts: " << sceneVerts.size() / 3 << std::endl;

	// Create the OpenGL render data
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);

	glBindVertexArray(vao);
	
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sceneVerts.size() * sizeof(GLfloat), sceneVerts.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sceneIndices.size() * sizeof(GLuint), sceneIndices.data(), GL_STATIC_DRAW);

	// Vertex positions
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);  
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);

	std::cout << "Successfully initialized buffers" << std::endl;
}

void Scene::OnInitialize() {
	shader = new Shader("shaders/vertex.glsl", "shaders/fragment.glsl");

	uniView = glGetUniformLocation(shader->handle, "view");
	uniProj = glGetUniformLocation(shader->handle, "proj");

}

void Scene::OnRender() {
	glUseProgram(shader->handle);


	glm::mat4 trans = camera.GetMatrix();
	glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(trans));

	glm::mat4 proj = glm::perspective((PI/4), appSpec->width / (float) appSpec->height, 1.0f, 5000.0f);
	glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(proj));

	GLint uniColor = glGetUniformLocation(shader->handle, "inColor");
	glm::vec3 colVec(0.f, 0.7f, 0.3f);
	glUniform3fv(uniColor, 1, glm::value_ptr(colVec));

	glBindVertexArray(vao);
	//glDrawArrays(GL_TRIANGLES, 0, sceneVerts.size() / 3);
	glDrawElements(GL_TRIANGLES, sceneIndices.size(), GL_UNSIGNED_INT, (void*)0);
	glBindVertexArray(0);
}

void Scene::OnTerminate() {
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ebo);
	glDeleteProgram(shader->handle);

	delete shader;
	delete bspFile;

	std::cout << "Successfully deleted buffers" << std::endl;
}

void Scene::OnMouseControl(float dx, float dy) {
	camera.AnglesRotate(dx, dy);
}