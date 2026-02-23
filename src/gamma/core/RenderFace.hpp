#include <vector>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/gl.h>

namespace GammaEngine {
	class RenderFace {
		public:
			RenderFace();
			~RenderFace() {}

			void SetIndices(std::vector<unsigned int> indices);

			void Draw();

		private:
			GLuint ebo_;

			std::vector<unsigned int> indices_;
			unsigned int frameNum_;
	};
}