#pragma once

#include <SFML/Window.hpp>
#include <string>

namespace GammaEngine {
	class Window {
	public:
		Window(std::string title = "Gamma Engine", unsigned int width = 800, unsigned int height = 600);
		~Window();

		std::string GetTitle();
		void SetTitle(std::string title);
		
	private:
		sf::Window		*window_;
		std::string		title_;
		unsigned int	width_;
		unsigned int	height_;
		bool			dirty_;
	};
	
}