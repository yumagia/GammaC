#pragma once

namespace GammaEngine {
	class Renderer {
		public:
			Renderer() {}
			~Renderer() {}
			void Initialize(unsigned int width, unsigned int height);
			void ResizeViewport(unsigned int width, unsigned int height);
			void Draw();

		private:
			unsigned int	width_;
			unsigned int	height_;
	};
}