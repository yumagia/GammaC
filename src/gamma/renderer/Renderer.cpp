#include "Renderer.hpp"

#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>

#include <vector>


namespace GammaEngine {
	void Renderer::Initialize(unsigned int width, unsigned int height) {
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
        glDepthMask(GL_TRUE);
        glClearDepth(1.f);

        glDisable(GL_LIGHTING);
        glDisable(GL_CULL_FACE);
	}

	void Renderer::ResizeViewport(unsigned int width, unsigned int height, float aspectRatio) {
		// TODO: More resize modes
		GLfloat newRatio;
		newRatio =  static_cast<float>(width) / static_cast<float>(height);
		
		if(newRatio > aspectRatio) {
			int viewportWidth = static_cast<float>(height) * aspectRatio;
			glViewport(		(static_cast<int>(width) - viewportWidth) / 2,
							0,
							viewportWidth,
							height	);
		}
		else {
			int viewportHeight = static_cast<float>(width) / aspectRatio;
			glViewport(		0,
							(static_cast<int>(height) - viewportHeight) / 2,
							width,
							viewportHeight	);
		}
	}

	void Renderer::Draw(Scene &scene) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//scene.Draw();
	}
}