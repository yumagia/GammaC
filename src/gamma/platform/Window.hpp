#pragma once

#include <GLFW/glfw3.h>
#include <string>

namespace GammaEngine {
	class Window {
	public:
		Window(std::string title = "Gamma Engine", unsigned int width = 800, unsigned int height = 600);
		~Window();

		bool CreateWindow();
		bool IsOpen();
		void HandleEvents();

		std::string		GetTitle();
		unsigned int	GetWidth();
		unsigned int	GetHeight();

		void SetTitle(std::string title);
		void SetWidth(unsigned int width);
		void SetHeight(unsigned int height);
		
	private:
		GLFWwindow		*window_;
		std::string		title_;
		unsigned int	width_;
		unsigned int	height_;
		bool			verticalSync_;
		bool			dirty_;
		unsigned int 	depthBits_;
		bool			srgbCapable_;
	};
	
}