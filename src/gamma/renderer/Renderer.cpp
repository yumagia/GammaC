#include "Renderer.hpp"

#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>

#include <vector>


namespace GammaEngine {
	void Renderer::Initialize(unsigned int width, unsigned int height) {

		ResizeViewport(width, height);
	}

	void Renderer::ResizeViewport(unsigned int width, unsigned int height) {
		glViewport(0, 0, static_cast<GLsizei>(width), static_cast<GLsizei>(height));
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		const GLfloat new_ratio =  static_cast<float>(width) / static_cast<float>(height);
		glFrustum(-new_ratio, new_ratio, -1.f, 1.f, 1.f, 500.f);

		width_ = width;
		height_ = height;
	}

	void Renderer::Draw(Scene &scene) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		scene.Draw();
	}
}