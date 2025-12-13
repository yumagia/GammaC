#ifndef APPLICATION_INCLUDED
#define APPLICATION_INCLUDED

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include "GammaFile.hpp"
#include "Scene.hpp"
#include "ApplicationSpecification.hpp"

#include <iostream>

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

	void Terminate();
	
private:
	Scene	*m_scene = NULL;

	GLint	uniView, uniProj;

	ApplicationSpecification	m_spec;
	SDL_Window					*m_window;
	SDL_GLContext				m_renderContext;
	SDL_Event					m_windowEvent;

	Uint64 lastTick, nowTick;
	double deltaTime;

	bool	m_quit = false;
	bool	m_fullscreen = false;
};

#endif