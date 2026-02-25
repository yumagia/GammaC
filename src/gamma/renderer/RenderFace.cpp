#include "RenderFace.hpp"

#include "ShaderLoader.hpp"

#include <GL/glew.h>
#include <GL/gl.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

namespace GammaEngine {
	RenderFace::RenderFace() {
		glGenBuffers(1, &ebo_);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);

		frameNum_ = 0;
	}

	RenderFace::~RenderFace() {
		if(ebo_ != 0) {
			glDeleteVertexArrays(1, &ebo_);
		}
	}

	void RenderFace::SetPlane(const FilePlane *plane) {
		plane_ = plane;
	}

	void RenderFace::SetIndices(std::vector<unsigned int> indices) {
		indices_ = indices;

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_.size() * sizeof(unsigned int), indices_.data(), GL_STATIC_DRAW);
	}

	void RenderFace::SetCurrentIndexCount(unsigned int currentIndexCount) {
		currentIndexCount_ = currentIndexCount;
	}
	
	unsigned int RenderFace::GetCurrentIndexCount() {
		return currentIndexCount_;
	}

	void RenderFace::Draw(unsigned int frameNum, Vec3f viewPosition) {
		if(frameNum_ == frameNum) {									// Already rendered
			return;
		}

		if(indices_.empty()) {
			return;
		}
		
		Vec3f normal(plane_->normal[0], plane_->normal[1], plane_->normal[2]);
		if(normal.Dot(viewPosition) - plane_->dist < 0) {			// BFC
			return;
		}
		frameNum_ = frameNum;

		glBufferSubData(	GL_ELEMENT_ARRAY_BUFFER, 
							currentIndexCount_ * sizeof(unsigned int), 
							indices_.size() * sizeof(unsigned int), 
							indices_.data()									);

		currentIndexCount_ += indices_.size();
	}
}