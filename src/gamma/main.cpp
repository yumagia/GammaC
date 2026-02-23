#include "MainLoop.hpp"
using namespace GammaEngine;

#include <iostream>

class Application: public MainLoop {
	public: 
		Application() {}
		~Application() {}
	private:
		void Configure() override;
		void Initialize() override;
		void Update(float deltaTime) override;
};

void Application::Configure() {
	window_.SetTitle("Gamma Engine");
}

void Application::Initialize() {
	scene_->LoadBspFile("testgi.txt");

	glClearColor(0.5f, 0.7f, 1.f, 0.f);
}

void Application::Update(float deltaTime) {

}

int main(int argv, char **argc) {
	std::cout << "--- GAMMA ENGINE ---" << std::endl;

	if(argv < 2) {
		std::cerr << "Usage is: ./GAMMA <bsp file name>" << std::endl;
		exit(1);
	}

	Application application;
	return application.Run();
}