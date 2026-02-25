#include <vector>

#include "GammaFile.hpp"
#include "Math.hpp"
#include "ShaderProgram.hpp"

#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/gl.h>

#include <glm/glm.hpp>

namespace GammaEngine {
	class RenderFace {
		public:
			RenderFace();
			~RenderFace();

			void SetPlane(const FilePlane *plane);
			void SetIndices(std::vector<unsigned int> indices);

			void Draw(unsigned int frameNum, Vec3f viewPosition);

			void SetCurrentIndexCount(unsigned int currentIndexCount);
			unsigned int GetCurrentIndexCount();

		private:

		private:
			GLuint ebo_;
			glm::vec4 color_;
			const FilePlane *plane_;
			
			std::vector<unsigned int> indices_;
			unsigned int currentIndexCount_;

			unsigned int frameNum_;
	};
}