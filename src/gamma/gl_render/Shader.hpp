#ifndef SHADER_INCLUDED
#define SHADER_INCLUDED

#include "glad/glad.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include <string>

class Shader {
public:
    Shader(const char *vertexPath, const char* fragmentPath);

    GLuint handle;

private:
    void CheckCompileErrors(GLuint shader, std::string type);
};

#endif