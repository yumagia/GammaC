#include "Window.hpp"

#include <iostream>

namespace GammaEngine {
	Window::Window(std::string title, unsigned int width, unsigned int height) {
		window_ = NULL;
		title_ = title;
		width_ = width;
		height_ = height;
		verticalSync_ = true;
		dirty_ = false;
		depthBits_ = 24;
		srgbCapable_ = true;
	}

	Window::~Window() {
		if(window_ != NULL) {
			glfwDestroyWindow(window_);
		}
	}

	std::string Window::GetTitle() {
		return title_;
	}

	unsigned int Window::GetWidth() {
		return width_;
	}

	unsigned int Window::GetHeight() {
		return height_;
	}

	void Window::SetWidth(unsigned int width) {
		width_ = width;
	}

	void Window::SetHeight(unsigned int height) {
		height_ = height;
	}

	void Window::SetTitle(std::string title) {
		title_ = title;
	}

	bool Window::CreateWindow() {
		window_ = glfwCreateWindow(width_, height_, title_.c_str(), NULL, NULL);
		if(window_ == NULL) {
			std::cout << "Failed to create GLFW window" << std::endl;
			glfwTerminate();
			return 0;
		}

		glfwMakeContextCurrent(window_);
		return 1;
	}

	bool Window::IsOpen() {
		return (window_ != NULL) && (!glfwWindowShouldClose(window_));
	}

	void Window::HandleEvents() {
		if(window_ == NULL) {
			std::cerr << "Error: window has not been created yet" << std::endl;
			return;
		}

		if(glfwGetKey(window_, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
			glfwSetWindowShouldClose(window_, true);
		}

		glfwPollEvents();
	}

	void Window::Display() {
		glfwSwapInterval(verticalSync_);
		glfwSwapBuffers(window_);
	}
}

