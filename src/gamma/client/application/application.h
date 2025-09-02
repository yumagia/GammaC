#pragma once

#include "imgui.h"
#include "vulkan/vulkan.h"

struct GLFWwindow;
struct ImGui_ImplVulkanH_Window;

class Application {
    public:
        void run();
    private:
        void init();
        void mainLoop();
		void cleanup();
    private:
        GLFWwindow* window = nullptr;
        ImGui_ImplVulkanH_Window* wd;
        ImVec4 clear_color;
        VkResult err;
};