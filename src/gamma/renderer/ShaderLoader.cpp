#include "ShaderLoader.hpp"

#include <stdexcept>

namespace GammaEngine {
	std::map<ShaderType, std::shared_ptr<ShaderProgram>> ShaderLoader::shaderCache_;

	std::shared_ptr<ShaderProgram> ShaderLoader::LoadShader(ShaderType shaderType) {
		switch(shaderType) {
			case ShaderType::DEFAULT:
			default:
				if(shaderCache_.find(shaderType) != shaderCache_.end()) {
					return shaderCache_[shaderType];
				}

				auto shader = std::make_shared<ShaderProgram>("shaders/default_shader.vert", "shaders/default_shader.frag");
				shaderCache_[shaderType] = shader;

				return shader;
		}
	}

	void ShaderLoader::ReleaseShader(ShaderType shaderType) {
		shaderCache_.erase(shaderType);
	}

	void ShaderLoader::ReleaseAllShaders() {
		shaderCache_.clear();
	}
}