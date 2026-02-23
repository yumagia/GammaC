#pragma once

#include "Scene.hpp"

namespace GammaEngine {
	class Renderer {
		public:
			Renderer() {}
			~Renderer() {}
			void Initialize(unsigned int width, unsigned int height);
			void ResizeViewport(unsigned int width, unsigned int height);
			void Draw(Scene &scene);

		private:
			unsigned int	width_;
			unsigned int	height_;
	};
}