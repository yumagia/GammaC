#pragma once

#include "vulkan/vulkan.h"

#include "backends/imgui_impl_vulkan.h"

#include <iostream>
#include <string>

namespace vkb {

	const std::string to_string(VkResult result);

}

namespace Gamma {

	ImGui_ImplVulkan_InitInfo* GetVulkanInfo();

}

#define VK_CHECK(x)                                                                    \
	do                                                                                 \
	{                                                                                  \
		VkResult err = x;                                                              \
		if (err)                                                                       \
		{                                                                              \
			std::cout << "Vulkan Error: " << vkb::to_string(err) << std::endl;         \
		}                                                                              \
	} while (0)