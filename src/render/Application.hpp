#ifndef APPLICATION_INCLUDED
#define APPLICATION_INCLUDED

#include <GLFW/glfw3.h>

#include "Scene.hpp"

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

private: 
    void InitializeGL();

    void BeginRendering();
    void RenderScene();

    void ProcessInput(GLFWwindow *window);
    
private:
    Scene scene;

    ApplicationSpecification m_spec;
    GLFWwindow *m_window;
};

#endif