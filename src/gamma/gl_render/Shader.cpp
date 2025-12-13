#include "Shader.hpp"

#include "glad/glad.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include <fstream>
#include <sstream>
#include <iostream>

Shader::Shader(const char *vertexPath, const char* fragmentPath) {
	std::cout << "--- Creating new shader ---" << std::endl;
	std::string vertexSource, fragmentSource;
	std::ifstream vShaderFile, fShaderFile;

	vShaderFile.open(vertexPath);
	fShaderFile.open(fragmentPath);
	if(!(vShaderFile.is_open() && fShaderFile.is_open())) {
		std::cerr << "Error opening shader files!" << std::endl;
		return;        
	}

	std::stringstream vShaderStream, fShaderStream;
	
	vShaderStream << vShaderFile.rdbuf();
	fShaderStream << fShaderFile.rdbuf();

	vShaderFile.close();
	fShaderFile.close();

	vertexSource = vShaderStream.str();
	fragmentSource = fShaderStream.str();


	const char *vShaderSource = vertexSource.c_str();
	const char *fShaderSource = fragmentSource.c_str();

	GLuint vertex, fragment;
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderSource, NULL);
	glCompileShader(vertex);
	CheckCompileErrors(vertex, "VERTEX");

	// Fragment Shader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderSource, NULL);
	glCompileShader(fragment);
	CheckCompileErrors(fragment, "FRAGMENT");

	// Shader Program
	handle = glCreateProgram();
	glAttachShader(handle, vertex);
	glAttachShader(handle, fragment);
	glLinkProgram(handle);
	CheckCompileErrors(handle, "PROGRAM");
	glDeleteShader(vertex);
	glDeleteShader(fragment);

	std::cout << "Done creating shaders" << std::endl;
}

void Shader::CheckCompileErrors(GLuint shader, std::string type) {
	int success;
	char infoLog[1024];
	if(type != "PROGRAM") {
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if(!success) {
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
		}
	}
	else {
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
		}
	}
}