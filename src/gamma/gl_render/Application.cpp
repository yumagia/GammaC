// Include GLAD first
#include "glad/glad.h"
#include "Application.hpp"

#include <GLFW/glfw3.h>

#include <iostream>		// For std::cerr

Application::Application() {
	InitializeGL();
}

Application::~Application() {
	glfwTerminate();
	exit(0);
}

void Application::InitializeGL() {
	glfwInit();

	// Set window hints
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create GLFW window
	m_window = glfwCreateWindow(m_spec.width, m_spec.height, m_spec.title, NULL, NULL);
	if(!m_window) {
		std::cerr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		exit(-1);
	}
	glfwMakeContextCurrent(m_window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		exit(-1);
	}   

	glfwSwapInterval(1);
}

void Application::BeginRendering() {
	// Set viewport
	int viewWidth, viewHeight;
	glfwGetFramebufferSize(m_window, &viewHeight, &viewHeight);
	glViewport(0, 0, viewWidth, viewHeight);

	// Clear the frame
	glClearColor(0.6f, 0.8f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// TODO: render buffers
	//unsigned int VBO;
	//glGenBuffers(1, &VBO);
	//glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
}

void Application::RenderScene() {

}

int Application::Run() {
	while(!glfwWindowShouldClose(m_window)) {
		// Process input and events
		ProcessInput(m_window);
		glfwPollEvents();

		BeginRendering();
		RenderScene();

		// Diplay the frame
		glfwSwapBuffers(m_window);
	}

	glfwTerminate();
	return 0;
}

// Process all input
void Application::ProcessInput(GLFWwindow *window) {
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)	// Close on escape
        glfwSetWindowShouldClose(window, true);
}