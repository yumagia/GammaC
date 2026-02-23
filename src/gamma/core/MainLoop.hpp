#pragma once

#include "Window.hpp"
#include "Renderer.hpp"
#include "Scene.hpp"
#include "Clock.hpp"

#include <memory>

namespace GammaEngine {
	class MainLoop {
		public:
			MainLoop();
			~MainLoop();
			int Run();

		private:
			bool GlfwInitialize();

		protected:
			virtual void Configure() = 0;
			virtual void Initialize() = 0;
			virtual void Update(float deltaTime) = 0;
			
			Window window_;
			Renderer renderer_;
			Clock clock_;
			std::shared_ptr<Scene> scene_;
	};
}