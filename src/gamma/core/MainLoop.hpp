#pragma once

#include "Window.hpp"
#include "Renderer.hpp"

namespace GammaEngine {
	class MainLoop {
		public:
			MainLoop();
			~MainLoop();
			int Run();

		protected:
			virtual void Initialize() = 0;
			Window window_;
			Renderer renderer_;
	};
}