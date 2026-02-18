#include "Window.hpp"

namespace GammaEngine {
	Window::Window(std::string title, unsigned int width, unsigned int height) {
		window_ = nullptr;
		title_ = title;
		width_ = width;
		height_ = height;
		dirty_ = false;
	}

	Window::~Window() {
		if(window_ != nullptr)
			delete window_;
	}

	std::string Window::GetTitle() {
		return title_;
	}

	void Window::SetTitle(std::string title) {
		title_ = title;
	}
}