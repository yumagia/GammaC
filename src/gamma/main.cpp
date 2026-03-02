#include "MainLoop.hpp"
using namespace GammaEngine;

#include <iostream>

class Application: public MainLoop {
	public: 
		Application() {}
		~Application() {}
	private:
		void Configure() override;
		void Initialize(int argv, char **argc) override;
		void Update(float deltaTime) override;
};

void Application::Configure() {
	window_.SetTitle("Gamma Engine");
}

void Application::Initialize(int argv, char **argc) {
	scene_->CreateDefaltCamera();

	scene_->LoadBspFile(argc[1]);

	glClearColor(0.5f, 0.7f, 1.f, 0.f);
}

void Application::Update(float deltaTime) {
	std::shared_ptr<Camera> camera = scene_->GetCamera();
	int moveSpeed = 2000;
	UiEvent event;

	while(window_.PollEvent(event)) {
		switch(event) {
			case UiEvent::W_PRESSED:
				camera->SetPosition(camera->GetPosition() + (camera->GetRotation()).RotateVector(Vec3f(0, 0, -moveSpeed * deltaTime)));
				break;
			case UiEvent::A_PRESSED:
				camera->SetPosition(camera->GetPosition() + (camera->GetRotation()).RotateVector(Vec3f(-moveSpeed * deltaTime, 0, 0)));
				break;
			case UiEvent::S_PRESSED:
				camera->SetPosition(camera->GetPosition() + (camera->GetRotation()).RotateVector(Vec3f(0, 0, moveSpeed * deltaTime)));
				break;
			case UiEvent::D_PRESSED:
				camera->SetPosition(camera->GetPosition() + (camera->GetRotation()).RotateVector(Vec3f(moveSpeed * deltaTime, 0, 0)));
				break;
			case UiEvent::Q_PRESSED:
				camera->SetPosition(camera->GetPosition() + (camera->GetRotation()).RotateVector(Vec3f(0, -moveSpeed * deltaTime, 0)));
				break;
			case UiEvent::E_PRESSED:
				camera->SetPosition(camera->GetPosition() + (camera->GetRotation()).RotateVector(Vec3f(0, moveSpeed * deltaTime, 0)));
				break;
			case UiEvent::UP_PRESSED:
				camera->SetRotation(camera->GetRotation() * Quaternion(Vec3f(1, 0, 0), 3.f * deltaTime));
				break;
			case UiEvent::DOWN_PRESSED:
				camera->SetRotation(camera->GetRotation() * Quaternion(Vec3f(1, 0, 0), -3.f * deltaTime));
				break;
			case UiEvent::LEFT_PRESSED:
				camera->SetRotation(Quaternion(Vec3f(0, 1, 0), 3.f * deltaTime) * camera->GetRotation());
				break;
			case UiEvent::RIGHT_PRESSED:
				camera->SetRotation(Quaternion(Vec3f(0, 1, 0), -3.f * deltaTime) * camera->GetRotation());
				break;
		}
	}

}

int main(int argv, char **argc) {
	std::cout << "--- GAMMA ENGINE ---" << std::endl;

	if(argv < 2) {
		std::cerr << "Usage is: ./GAMMA <bsp file name>" << std::endl;
		exit(1);
	}

	Application application;
	return application.Run(argv, argc);
}