#include "ShaderProgram.hpp"

#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace GammaEngine {
	unsigned int CompileShader(unsigned int type, const std::string& src) {
		GLuint shader = glCreateShader(type);
		const char* srcCstr = src.c_str();
		glShaderSource(shader, 1, &srcCstr, nullptr);
		glCompileShader(shader);

		GLint success;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if(!success) {
			char log[512];
			glGetShaderInfoLog(shader, 512, nullptr, log);
			throw std::runtime_error(std::string("Shader compile error: ") + log);
		}

		return shader;
	}

	ShaderProgram::ShaderProgram(const std::string& vertPath, const std::string& fragPath) {
		std::string vertSrc, fragSrc;
		std::ifstream vShaderFile, fShaderFile;

		vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		try {
			vShaderFile.open(vertPath);
			fShaderFile.open(fragPath);

			std::stringstream vShaderStream, fShaderStream;
			// read file's buffer contents into streams
			vShaderStream << vShaderFile.rdbuf();
			fShaderStream << fShaderFile.rdbuf();		
			// close file handlers
			vShaderFile.close();
			fShaderFile.close();
			// convert stream into string
			vertSrc   = vShaderStream.str();
			fragSrc = fShaderStream.str();	
		}
		catch(std::ifstream::failure e) {
			std::cout << "SHADER PROGRAM ERROR: FILE NOT SUCCESFULLY READ" << std::endl;
		}

		unsigned int vert = CompileShader(GL_VERTEX_SHADER, vertSrc);
        unsigned int frag = CompileShader(GL_FRAGMENT_SHADER, fragSrc);

        programId_ = glCreateProgram();
        glAttachShader(programId_, vert);
        glAttachShader(programId_, frag);
        glLinkProgram(programId_);

        glDeleteShader(vert);
        glDeleteShader(frag);

		GLint success;
        glGetProgramiv(programId_, GL_LINK_STATUS, &success);
        if(!success) {
            char log[512];
            glGetProgramInfoLog(programId_, 512, nullptr, log);
            throw std::runtime_error(std::string("Shader link error: ") + log);
        }

		glGenBuffers(1, &matricesUbo_);
		glBindBuffer(GL_UNIFORM_BUFFER, matricesUbo_);
		glBufferData(GL_UNIFORM_BUFFER, 64 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_UNIFORM_BUFFER, 0, matricesUbo_);

		// Material UBO (binding = 1): material_color + use_texture + padding
		glGenBuffers(1, &materialUbo_);
		glBindBuffer(GL_UNIFORM_BUFFER, materialUbo_);
		glBufferData(GL_UNIFORM_BUFFER, 8 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_UNIFORM_BUFFER, 1, materialUbo_);

		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	ShaderProgram::~ShaderProgram() {
		if(programId_ != 0) {
			glDeleteProgram(programId_);
		}
	}

	ShaderProgram::ShaderProgram(ShaderProgram&& other) noexcept : programId_(other.programId_) {
		other.programId_ = 0;	// Prevent double-delete
	}

	ShaderProgram& ShaderProgram::operator=(ShaderProgram&& other) noexcept {
		if(this != &other) {
			if(programId_ != 0)
				glDeleteProgram(programId_);
			programId_ = other.programId_;
			other.programId_ = 0;
		}
		return *this;
	}

	void ShaderProgram::SetLightTexture(unsigned int width, unsigned int height, unsigned char *data) {
		glGenTextures(1, &lightTextureID_);
		glBindTexture(GL_TEXTURE_2D, lightTextureID_);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	}

	void ShaderProgram::SetBaseTexture(unsigned int width, unsigned int height, unsigned char *data) {
		glGenTextures(1, &baseTextureID_);
		glBindTexture(GL_TEXTURE_2D, baseTextureID_);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	}

	unsigned int ShaderProgram::GetBaseTexture() const {
		return baseTextureID_;
	}

	unsigned int ShaderProgram::GetLightTexture() const {
		return lightTextureID_;
	}

	void ShaderProgram::Bind() const {
		glUseProgram(programId_);
	}

	void ShaderProgram::Unbind() const {
		glUseProgram(0);
	}

	unsigned int ShaderProgram::GetId() const {
		return programId_;
	}

	unsigned int ShaderProgram::GetMatricesUbo() const { 
		return matricesUbo_; 
	}

	unsigned int ShaderProgram::GetMaterialUbo() const { 
		return materialUbo_; 
	}
}