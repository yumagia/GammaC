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
			int Run(int argv, char **argc);

		private:
			bool GlfwInitialize();

			void HandleResize();

		protected:
			virtual void Configure() = 0;
			virtual void Initialize(int argv, char **argc) = 0;
			virtual void Update(float deltaTime) = 0;

			unsigned int mainLoopTicksElapsed_;
			
			Window window_;
			Renderer renderer_;
			Clock clock_;
			std::shared_ptr<Scene> scene_;
	};
}