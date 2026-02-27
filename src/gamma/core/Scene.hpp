#pragma once

#define MAX_EBO 1000000

#include "GammaFile.hpp"

#include "RenderFace.hpp"
#include "Camera.hpp"
#include "Atlas.hpp"

#include <memory>
#include <vector>
#include <string>
#include <functional>

namespace GammaEngine {
	class Scene {
		public:
			Scene();
			~Scene();

			void LoadBspFile(std::string fileName);
			void CreateDefaltCamera();
			
			std::shared_ptr<Camera> GetCamera();

			void Update(float deltaTime);
			void Draw();

		private:
			void GenerateLightmapAtlas();

			void BindShader(const ShaderProgram& shaderProgram, const glm::mat4 &worldMatrix, const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix);
			void UnbindShader(const ShaderProgram& shaderProgram);

			void DrawLeaf(int leafIdx);
			void DrawWorldNodeRecursive(int childNodeIdx);

		private:
			std::shared_ptr<GammaEngine::ShaderProgram> shaderProgram_;

			GLuint vao_, vbo_, ebo_;
			std::vector<float> buffer_;
			unsigned int currentIndexCount_;

			std::function<void()> gpuRelease_{0};

			std::shared_ptr<Atlas> atlas_{nullptr};
			std::shared_ptr<Camera> camera_{nullptr};

			std::vector<RenderFace> renderFaces_;
			unsigned int frameNum_;

			std::vector<FileModel> models_;
			std::vector<FileEntity> entities_;
			std::vector<FilePlane> planes_;
			std::vector<FileNode> nodes_;
			std::vector<FileLeaf> leafs_;
			std::vector<unsigned int> leafFaces_;
			std::vector<FileVert> verts_;
			std::vector<unsigned int> faceVerts_;
			std::vector<FileFace> faces_;
			std::vector<FileMaterial> materials_;
			std::vector<FileLumel> lightmaps_;

			int nodesTraversed_;
	};
}