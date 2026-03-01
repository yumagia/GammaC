#pragma once

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <vector>

namespace GammaEngine {
	enum {
		GAMMA_ENGINE_KEY_W_PRESSED,
		GAMMA_ENGINE_KEY_A_PRESSED,
		GAMMA_ENGINE_KEY_S_PRESSED,
		GAMMA_ENGINE_KEY_D_PRESSED,
		GAMMA_ENGINE_KEY_UP_PRESSED,
		GAMMA_ENGINE_KEY_DOWN_PRESSED,
		GAMMA_ENGINE_KEY_LEFT_PRESSED,
		GAMMA_ENGINE_KEY_RIGHT_PRESSED
	};

	class Window {
	public:
		Window(std::string title = "Gamma Engine", unsigned int width = 800, unsigned int height = 600);
		~Window();

		bool CreateWindow();
		bool IsOpen();
		void HandleEvents();
		void Display();

		bool PollEvent(int &event);

		std::string		GetTitle();
		unsigned int	GetWidth();
		unsigned int	GetHeight();

		void SetTitle(std::string title);
		void SetWidth(unsigned int width);
		void SetHeight(unsigned int height);

		bool GetDirty();
		void SetDirty(bool dirty);

	private:
		std::vector<int>	events_;

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