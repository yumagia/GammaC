// Include GLAD first
#include "glad/glad.h"
#include "Application.hpp"
#include "Scene.hpp"

#include <SDL2/SDL.h>

#include <iostream>		// For std::cerr

Application::Application() {
	m_quit = false;
	m_scene = new Scene();
}

Application::~Application() {
	SDL_GL_DeleteContext(m_renderContext);
	SDL_Quit();
	exit(0);
	delete m_scene;
}

void Application::Initialize() {
	InitializeGL();
}

void Application::ReadBspFile(std::string fileName) {
	m_scene->LoadFile(fileName);
}

void Application::InitializeGL() {
	SDL_Init(SDL_INIT_VIDEO);  


	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

	m_window = SDL_CreateWindow(m_spec.title, m_spec.width, m_spec.height, m_spec.width, m_spec.height, SDL_WINDOW_OPENGL);
	m_renderContext = SDL_GL_CreateContext(m_window);
	
	if(!gladLoadGLLoader(SDL_GL_GetProcAddress)) {
		std::cout << "ERROR: Failed to initialize OpenGL context" << std::endl;
		return;
	}
}

void Application::WindowResize(int newWidth, int newHeight) {
	m_spec.width = newWidth;
	m_spec.height = newHeight;

	glViewport(0, 0, newWidth, newHeight);
}

void Application::BeginRendering() {
	// Clear the frame
	glClearColor(0.6f, 0.8f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
}

void Application::FinishRendering() {
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
}

void Application::RenderScene() {

}

int Application::Run() {
	while(!m_quit) {
		while(SDL_PollEvent(&m_windowEvent)) {
			if(m_windowEvent.type == SDL_QUIT) {
				m_quit = true;
			}
			if(m_windowEvent.type == SDL_KEYUP && m_windowEvent.key.keysym.sym == SDLK_ESCAPE) {
				m_quit = true;
			}

		}

		BeginRendering();
		RenderScene();
		FinishRendering();

		// Diplay the frame
		SDL_GL_SwapWindow(m_window);
	}

	SDL_GL_DeleteContext(m_renderContext);
	SDL_Quit();
	return 0;
}
