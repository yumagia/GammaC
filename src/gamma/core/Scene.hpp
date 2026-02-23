#pragma once

#include "GammaFile.hpp"

#include "RenderFace.hpp"
#include "Camera.hpp"

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
			
			void Update(float deltaTime);
			void Draw();

		private:
			void DrawLeaf(int leafIdx);
			void DrawWorldNodeRecursive(int childNodeIdx);

		private:
			GLuint vao_, vbo_;
			std::vector<float> buffer_;

			std::function<void()> gpuRelease_{0};

			Camera camera_;
			Transform view_;

			std::vector<RenderFace> renderFaces_;

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