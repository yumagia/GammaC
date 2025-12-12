#ifndef APPLICATION_INCLUDED
#define APPLICATION_INCLUDED

#include <SDL2/SDL.h>
#include "GammaFile.hpp"
#include "Scene.hpp"

#include <iostream>

struct ApplicationSpecification {
	int width = 800;
	int height = 600;
	const char *title = "GRAK";
};

class Application {
public:
	Application();
	~Application();

	int Run();
	void ReadBspFile(std::string fileName);
	void Initialize();

private: 
	void InitializeGL();

	void WindowResize(int newWidth, int newHeight);


	void BeginRendering();
	void FinishRendering();
	void RenderScene();
	
private:
	Scene     					*m_scene = NULL;

	ApplicationSpecification	m_spec;
	SDL_Window					*m_window;
	SDL_GLContext				m_renderContext;
	SDL_Event					m_windowEvent;
	bool						m_quit = false;
};

#endif