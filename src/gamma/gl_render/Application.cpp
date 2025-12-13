// Include GLAD first
#include "glad/glad.h"
#include "Application.hpp"
#include "Scene.hpp"
#include "Shader.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include <iostream>		// For std::cerr

Application::Application() {
	m_quit = false;
	m_scene = new Scene(&m_spec);

	InitializeGL();
}

Application::~Application() {
	SDL_GL_DeleteContext(m_renderContext);
	SDL_Quit();

	delete m_scene;
}

void Application::Initialize() {
	m_scene->OnInitialize();
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

	SDL_SetRelativeMouseMode(SDL_TRUE);
	SDL_ShowCursor(SDL_DISABLE);
	SDL_SetWindowGrab(m_window, SDL_TRUE);
}

void Application::WindowResize(int newWidth, int newHeight) {
	m_spec.width = newWidth;
	m_spec.height = newHeight;
}

void Application::BeginRendering() {
	// Clear the frame
	glClearColor(0.6f, 0.8f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
}

void Application::FinishRendering() {
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
}

void Application::RenderScene() {
	m_scene->OnRender();
}

void Application::Terminate() {
	m_scene->OnTerminate();

	SDL_GL_DeleteContext(m_renderContext);
	SDL_Quit();
}

int Application::Run() {
	while(!m_quit) {
		while(SDL_PollEvent(&m_windowEvent)) {
			if(m_windowEvent.type == SDL_QUIT) {
				m_quit = true;
			}
			if(m_windowEvent.type == SDL_KEYUP && m_windowEvent.key.keysym.sym == SDLK_ESCAPE) {
				SDL_SetRelativeMouseMode(SDL_FALSE);
				SDL_ShowCursor(SDL_ENABLE);
				SDL_SetWindowGrab(m_window, SDL_FALSE);
			}
			if(m_windowEvent.type == SDL_MOUSEMOTION) {
				int dx = m_windowEvent.motion.xrel;
				int dy = m_windowEvent.motion.yrel;
				m_scene->OnMouseControl(dx * m_spec.mouseSens, dy * m_spec.mouseSens);
			}

			if (m_windowEvent.type == SDL_KEYUP && m_windowEvent.key.keysym.sym == SDLK_f){
				m_fullscreen = !m_fullscreen;
				SDL_SetWindowFullscreen(m_window, m_fullscreen ? SDL_WINDOW_FULLSCREEN : 0);
			}

			// Alt controls
			if(m_windowEvent.type == SDL_KEYDOWN && m_windowEvent.key.keysym.sym == SDLK_UP) {
				m_scene->OnMouseControl(0, deltaTime * m_spec.keySens);
			}
			if(m_windowEvent.type == SDL_KEYDOWN && m_windowEvent.key.keysym.sym == SDLK_DOWN) {
				m_scene->OnMouseControl(0, -deltaTime * m_spec.keySens);
			}
			if(m_windowEvent.type == SDL_KEYDOWN && m_windowEvent.key.keysym.sym == SDLK_LEFT) {
				m_scene->OnMouseControl(deltaTime * m_spec.keySens, 0);
			}
			if(m_windowEvent.type == SDL_KEYDOWN && m_windowEvent.key.keysym.sym == SDLK_RIGHT) {
				m_scene->OnMouseControl(-deltaTime * m_spec.keySens, 0);
			}

			if(m_windowEvent.type == SDL_KEYDOWN && m_windowEvent.key.keysym.sym == SDLK_w) {
				m_scene->OnCameraMove(2, deltaTime * m_spec.moveSpeed);
			}
			if(m_windowEvent.type == SDL_KEYDOWN && m_windowEvent.key.keysym.sym == SDLK_a) {
				m_scene->OnCameraMove(0, -deltaTime * m_spec.moveSpeed);
			}
			if(m_windowEvent.type == SDL_KEYDOWN && m_windowEvent.key.keysym.sym == SDLK_s) {
				m_scene->OnCameraMove(2, -deltaTime * m_spec.moveSpeed);
			}
			if(m_windowEvent.type == SDL_KEYDOWN && m_windowEvent.key.keysym.sym == SDLK_d) {
				m_scene->OnCameraMove(0, deltaTime * m_spec.moveSpeed);
			}

		}

		lastTick = nowTick;
		nowTick = SDL_GetPerformanceCounter();

		deltaTime = (double)((nowTick - lastTick) / (double) SDL_GetPerformanceFrequency());

		BeginRendering();
		RenderScene();
		FinishRendering();

		// Diplay the frame
		SDL_GL_SwapWindow(m_window);
	}

	Terminate();

    std::cout << "Successfully closed application" << std::endl;
	return 0;
}
