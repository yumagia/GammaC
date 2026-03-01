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

	bool Window::GetDirty() {
		return dirty_;
	}

	void Window::SetDirty(bool dirty) {
		dirty_ = dirty;
	}

	bool Window::CreateWindow() {
		window_ = glfwCreateWindow(width_, height_, title_.c_str(), NULL, NULL);
		if(window_ == NULL) {
			std::cout << "Failed to create GLFW window" << std::endl;
			glfwTerminate();
			return 0;
		}

		glfwMakeContextCurrent(window_);
		glfwSwapInterval(verticalSync_);
		return 1;
	}

	bool Window::IsOpen() {
		return (window_ != NULL) && (!glfwWindowShouldClose(window_));
	}

	void Window::HandleEvents() {
		events_.clear();

		if(window_ == NULL) {
			std::cerr << "Error: window has not been created yet" << std::endl;
			return;
		}

		if(glfwGetKey(window_, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
			glfwSetWindowShouldClose(window_, true);
		}

		if(glfwGetKey(window_, GLFW_KEY_W) == GLFW_PRESS) {
			events_.push_back(GAMMA_ENGINE_KEY_W_PRESSED);
		}
		if(glfwGetKey(window_, GLFW_KEY_A) == GLFW_PRESS) {
			events_.push_back(GAMMA_ENGINE_KEY_A_PRESSED);
		}
		if(glfwGetKey(window_, GLFW_KEY_S) == GLFW_PRESS) {
			events_.push_back(GAMMA_ENGINE_KEY_S_PRESSED);
		}
		if(glfwGetKey(window_, GLFW_KEY_D) == GLFW_PRESS) {
			events_.push_back(GAMMA_ENGINE_KEY_D_PRESSED);
		}
		if(glfwGetKey(window_, GLFW_KEY_UP) == GLFW_PRESS) {
			events_.push_back(GAMMA_ENGINE_KEY_UP_PRESSED);
		}
		if(glfwGetKey(window_, GLFW_KEY_DOWN) == GLFW_PRESS) {
			events_.push_back(GAMMA_ENGINE_KEY_DOWN_PRESSED);
		}
		if(glfwGetKey(window_, GLFW_KEY_LEFT) == GLFW_PRESS) {
			events_.push_back(GAMMA_ENGINE_KEY_LEFT_PRESSED);
		}
		if(glfwGetKey(window_, GLFW_KEY_RIGHT) == GLFW_PRESS) {
			events_.push_back(GAMMA_ENGINE_KEY_RIGHT_PRESSED);
		}

		int newWidth, newHeight;
		glfwGetFramebufferSize(window_, &newWidth, &newHeight);
		if(newWidth != width_ || newHeight != height_) {
			width_ = newWidth;
			height_ = newHeight;
			dirty_ = true;
		}

		glfwPollEvents();
	}

	void Window::Display() {
		glfwSwapBuffers(window_);
	}

	bool Window::PollEvent(int &event) {
		if(events_.empty()) {
			return false;
		}
		event = events_.back();
		events_.pop_back();

		return true;
	}
}

