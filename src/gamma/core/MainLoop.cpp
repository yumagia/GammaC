#include "MainLoop.hpp"

#include <iostream>

namespace GammaEngine {
	MainLoop::MainLoop() {
		scene_ = std::make_shared<Scene>();
	}

	MainLoop::~MainLoop() {

	}

	bool MainLoop::GlfwInitialize() {
		GLenum error = glfwInit();
		error = glewInit();
		if(error != GLFW_TRUE) {
			return 0;
		}
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		return 1;
	}

	int MainLoop::Run(int argv, char **argc) {
		if(!GlfwInitialize()) {
			std::cout << "Failed to initialize GLFW" << std::endl;
			return EXIT_FAILURE;
		}

		Configure();

		if(!window_.CreateWindow()) {
			return EXIT_FAILURE;
		}

		GLenum error = glewInit();
		if(error != GLEW_OK) {
			std::cout << "Failed to initialize GLEW" << std::endl;
			return EXIT_FAILURE;
		}

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		Initialize(argv, argc);

		renderer_.Initialize(window_.GetWidth(), window_.GetHeight());

		clock_.Reset();

		while(window_.IsOpen())	{
			window_.HandleEvents();

			float deltaTime = clock_.DeltaTime();
			std::cout << 1.f / deltaTime << std::endl;

			Update(deltaTime);

			renderer_.Draw(*scene_);
			window_.Display();
		}
		
		return EXIT_SUCCESS;
	}
}