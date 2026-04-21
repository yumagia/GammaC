#pragma once

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <vector>

namespace GammaEngine {
	enum class UiEvent {
		W_PRESSED,
		A_PRESSED,
		S_PRESSED,
		D_PRESSED,
		Q_PRESSED,
		E_PRESSED,
		UP_PRESSED,
		DOWN_PRESSED,
		LEFT_PRESSED,
		RIGHT_PRESSED
	};

	class Window {
	public:
		Window(std::string title = "Gamma Engine", unsigned int width = 1200, unsigned int height = 900);
		~Window();

		bool CreateWindow();
		bool IsOpen();
		void HandleEvents();
		void Display();

		bool PollEvent(UiEvent &event);

		std::string		GetTitle();
		unsigned int	GetWidth();
		unsigned int	GetHeight();

		void SetTitle(std::string title);
		void SetWidth(unsigned int width);
		void SetHeight(unsigned int height);

		bool GetDirty();
		void SetDirty(bool dirty);

	private:
		std::vector<UiEvent>	events_;

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