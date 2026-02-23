#include "RenderFace.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

namespace GammaEngine {
	RenderFace::RenderFace() {
		glGenBuffers(1, &ebo_);

		frameNum_ = 0;
	}

	void RenderFace::SetIndices(std::vector<unsigned int> indices) {
		indices_ = indices;

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_.size() * sizeof(unsigned int), indices_.data(), GL_STATIC_DRAW);
	}

	void RenderFace::Draw() {
		if(indices_.empty()) {
			return;
		}

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glDrawElements(GL_TRIANGLES, indices_.size(), GL_UNSIGNED_INT, (void*)0);
	}
}