#pragma once

#include <string>

namespace GammaEngine {
	class ShaderProgram {
		public:
			ShaderProgram(const std::string& vertSrc, const std::string& fragSrc);
			~ShaderProgram();

			ShaderProgram(const ShaderProgram&) = delete;
			ShaderProgram& operator=(const ShaderProgram&) = delete;
			ShaderProgram(ShaderProgram&& other) noexcept;
			ShaderProgram& operator=(ShaderProgram&& other) noexcept;

			void Bind() const;
			void Unbind() const;

			void SetBaseTexture(unsigned int width, unsigned int height, unsigned char *data);
			void SetLightTexture(unsigned int width, unsigned int height, unsigned char *data);
			
			unsigned int GetBaseTexture() const;
			unsigned int GetLightTexture() const;

			unsigned int GetId() const;
			unsigned int GetMatricesUbo() const;
			unsigned int GetMaterialUbo() const;
			
		private:
			unsigned int programId_{0};
			unsigned int matricesUbo_{0};
			unsigned int materialUbo_{0};

			unsigned int baseTextureID_{0};
			unsigned int lightTextureID_{0};
	};
}