#include "renderer.h"

#include "application.h"

#include <glm/gtc/matrix_transform.hpp>
#include <array>
#include <fstream>
#include "imgui.h"
#include "imgui_internal.h"
#include "misc/cpp/imgui_stdlib.h"

namespace Renderer {
	static uint32_t ImGui_ImplVulkan_MemoryType(VkMemoryPropertyFlags properties, uint32_t type_bits)
	{
		VkPhysicalDevice physicalDevice = Renderer::GetVulkanInfo()->PhysicalDevice;

		VkPhysicalDeviceMemoryProperties prop;
		vkGetPhysicalDeviceMemoryProperties(physicalDevice, &prop);
		for (uint32_t i = 0; i < prop.memoryTypeCount; i++)
			if ((prop.memoryTypes[i].propertyFlags & properties) == properties && type_bits & (1 << i))
				return i;
		return 0xFFFFFFFF; // Unable to find memoryType
	}

	void Renderer::Init()
	{
		InitBuffers();
		InitPipeline();
	}

	void Renderer::Shutdown()
	{

	}

	void Renderer::Render()
	{
		VkCommandBuffer commandBuffer = Gamma::Application::GetActiveCommandBuffer();
		auto wd = Gamma::Application::GetMainWindowData();

		float viewportWidth = (float)wd->Width;
		float viewportHeight = (float)wd->Height;

		// Bind the graphics pipeline.
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_GraphicsPipeline);

		vkCmdPushConstants(commandBuffer, m_PipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConstants), &m_PushConstants);

		VkDeviceSize offset{ 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, &m_VertexBuffer.Handle, &offset);

		vkCmdBindIndexBuffer(commandBuffer, m_IndexBuffer.Handle, offset, VK_INDEX_TYPE_UINT32);

		VkViewport vp{};
		vp.y = viewportHeight;
		vp.width = viewportWidth;
		vp.height = -viewportHeight;
		vp.minDepth = 0.0f;
		vp.maxDepth = 1.0f;
		vkCmdSetViewport(commandBuffer, 0, 1, &vp);

		VkRect2D scissor{};
		scissor.extent.width = wd->Width;
		scissor.extent.height = wd->Height;
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

		vkCmdDrawIndexed(commandBuffer, 36, 1, 0, 0, 0);
	}

}