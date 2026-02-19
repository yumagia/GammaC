#include "MainLoop.hpp"

#include <iostream>

namespace GammaEngine {
	MainLoop::MainLoop() {

	}

	MainLoop::~MainLoop() {
		
	}

	bool MainLoop::GlfwInitialize() {
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		return 1;
	}

	int MainLoop::Run() {
		if(!GlfwInitialize()) {
			return EXIT_FAILURE;
		}
		Initialize();

		if(!window_.CreateWindow()) {
			return EXIT_FAILURE;
		}

		renderer_.Initialize(window_.GetWidth(), window_.GetHeight());

		while(window_.IsOpen())	{
			window_.HandleEvents();

			renderer_.Draw();
		}
		
		return EXIT_SUCCESS;
	}
}