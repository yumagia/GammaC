#pragma once

#include <string>

#include "imgui.h"
#include "vulkan/vulkan.h"

struct GLFWwindow;
struct ImGui_ImplVulkanH_Window;

struct ApplicationSpecification {
    std::string Name = "Gamma";
    uint32_t Width = 1600;
    uint32_t Height = 900;
};

class Application {
    public:
        Application(const ApplicationSpecification& applicationSpecification = ApplicationSpecification());
		~Application();
        void Run();
    private:
        void Init();
        void MainLoop();
		void Cleanup();
        void RenderView();
    private:
        ApplicationSpecification m_Specification;
        GLFWwindow* window = nullptr;
        ImGui_ImplVulkanH_Window* wd;
        ImVec4 clear_color;
        VkResult err;
};
