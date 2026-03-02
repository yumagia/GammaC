#pragma once

#include "Scene.hpp"
#include "Clock.hpp"

namespace GammaEngine {
	class Renderer {
		public:
			Renderer() {}
			~Renderer() {}
			void Initialize(unsigned int width, unsigned int height);
			void ResizeViewport(unsigned int width, unsigned int height, float aspectRatio);
			void Draw(Scene &scene);
		private:
			Clock profiler_;
	};
}