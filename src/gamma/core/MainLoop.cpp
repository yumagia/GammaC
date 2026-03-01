#include "MainLoop.hpp"

#include <iostream>
#include <memory>

namespace GammaEngine {
	MainLoop::MainLoop() {
		scene_ = std::make_shared<Scene>();
	}

	MainLoop::~MainLoop() {

	}

	bool MainLoop::GlfwInitialize() {
		GLenum error = glfwInit();
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

		Initialize(argv, argc);

		renderer_.Initialize(window_.GetWidth(), window_.GetHeight());

		clock_.Reset();
		Clock intervalClock;
		intervalClock.Reset();
		mainLoopTicksElapsed_ = 0;
		while(window_.IsOpen())	{
			window_.HandleEvents();

			float deltaTime = clock_.DeltaTime();
			// std::cout << 1.f / deltaTime << std::endl;

			Update(deltaTime);

			std::shared_ptr<Camera> camera = scene_->GetCamera();
			camera->Update(deltaTime);

			HandleResize();

			renderer_.Draw(*scene_);
			window_.Display();

			if((mainLoopTicksElapsed_ % 120) == 0) {
				std::cout << 120.f / intervalClock.DeltaTime() << std::endl;
				intervalClock.Reset();
			}
			mainLoopTicksElapsed_++;
		}
		
		return EXIT_SUCCESS;
	}

	void MainLoop::HandleResize() {
		std::shared_ptr<Camera> camera = scene_->GetCamera();

		if(camera) {
			if(window_.GetDirty() || camera->GetProjectionMatrixDirty()) {
				renderer_.ResizeViewport(	
											window_.GetWidth(),
											window_.GetHeight(),
											camera->GetAspectRatio()	);

				window_.SetDirty(false);
				camera->SetProjectionMatrixDirty(false);
			}
		}
		else {
			if(window_.GetDirty()) {
				renderer_.ResizeViewport(	
											window_.GetWidth(),
											window_.GetHeight(),
											camera->GetAspectRatio()	);
			}

			window_.SetDirty(false);
		}
	}
}