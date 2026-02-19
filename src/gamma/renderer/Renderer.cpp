#include "Renderer.hpp"

#include <GL/gl.h>
#include <GLFW/glfw3.h>

#include <vector>


namespace GammaEngine {
	void Renderer::Initialize(unsigned int width, unsigned int height) {
		GLuint pixelData[width * height * 4];

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

	struct Pixel {
		uint8_t r{ 0 }, g{ 0 }, b{ 0 }, a{ 0 };
	};
	void Renderer::Draw() {

	}
}