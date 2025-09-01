#pragma once

#include "vulkan.h"

#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>

#include <filesystem>

namespace Renderer {

	struct Buffer {
		VkBuffer Handle = nullptr;
		VkDeviceMemory Memory = nullptr;
		VkDeviceSize Size = 0;
		VkBufferUsageFlagBits Usage = VK_BUFFER_USAGE_FLAG_BITS_MAX_ENUM;
	};

	class Renderer {
	public:
		void Init();
		void Shutdown();

		void Render();
	private:
		void InitPipeline();
		void InitBuffers();

		void CreateOrResizeBuffer(Buffer& buffer, uint64_t newSize);

		VkShaderModule LoadShader(const std::filesystem::path& path);
	private: 
		VkPipeline m_GraphicsPipeline = nullptr;
		VkPipelineLayout m_PipelineLayout = nullptr;

		Buffer m_VertexBuffer, m_IndexBuffer;

		struct PushConstants {
			glm::mat4 ViewProjection;
			glm::mat4 Transform;
		} m_PushConstants;
	};
}