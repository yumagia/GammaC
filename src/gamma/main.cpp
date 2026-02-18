#include "MainLoop.hpp"
using namespace GammaEngine;

#include <iostream>

class Application: public MainLoop {
	public: 
		Application() {}
		~Application() {}
	private:
		void Initialize() override;
};

void Application::Initialize() {
	window_.SetTitle("Gamma Engine");
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