#pragma once

namespace GammaEngine {
	class Renderer {
		public:
			Renderer() {}
			~Renderer() {}
			void Initialize(unsigned int width, unsigned int height);
			void ResizeViewport(unsigned int width, unsigned int height);
			void Draw(unsigned int width, unsigned int height);
	};
}