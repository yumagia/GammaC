#pragma once

#include "ShaderProgram.hpp"

#include <memory>
#include <map>

namespace GammaEngine {
    enum class ShaderType {
        DEFAULT
    };

    class ShaderLoader {
        public:
            static std::shared_ptr<ShaderProgram> LoadShader(ShaderType shaderType);
            static void ReleaseShader(ShaderType shaderType);
            static void ReleaseAllShaders();

        private:
            static std::map<ShaderType, std::shared_ptr<ShaderProgram>> shaderCache_;
    };
}