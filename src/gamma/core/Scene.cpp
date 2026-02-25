#include "Scene.hpp"

#include "FileReader.hpp"
#include "ShaderLoader.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

namespace GammaEngine {
	Scene::Scene() {
		frameNum_ = 0;
	}

	Scene::~Scene() {
		if(gpuRelease_) {
			gpuRelease_();
		}
	}

	void Scene::LoadBspFile(std::string fileName) {
		FileReader fileReader;
		BspFile *bspFile = fileReader.ReadFile(fileName);

		if(!bspFile) {
			return;
		}

		int lumpLength;

		lumpLength = bspFile->fileHeader.lumps[LUMP_MODELS].length;
		models_.reserve(lumpLength);
		for(int i = 0; i < lumpLength; i++) {
			models_.push_back(bspFile->fileModels[i]);
		}
		lumpLength = bspFile->fileHeader.lumps[LUMP_ENTITIES].length;
		entities_.reserve(lumpLength);
		for(int i = 0; i < lumpLength; i++) {
			entities_.push_back(bspFile->fileEntities[i]);
		}
		lumpLength = bspFile->fileHeader.lumps[LUMP_PLANES].length;
		planes_.reserve(lumpLength);
		for(int i = 0; i < lumpLength; i++) {
			planes_.push_back(bspFile->filePlanes[i]);
		}
		lumpLength = bspFile->fileHeader.lumps[LUMP_NODES].length;
		nodes_.reserve(lumpLength);
		for(int i = 0; i < lumpLength; i++) {
			nodes_.push_back(bspFile->fileNodes[i]);
		}
		lumpLength = bspFile->fileHeader.lumps[LUMP_LEAFS].length;
		leafs_.reserve(lumpLength);
		for(int i = 0; i < lumpLength; i++) {
			leafs_.push_back(bspFile->fileLeafs[i]);
		}
		lumpLength = bspFile->fileHeader.lumps[LUMP_LEAFFACES].length;
		leafFaces_.reserve(lumpLength);
		for(int i = 0; i < lumpLength; i++) {
			leafFaces_.push_back(bspFile->fileLeafFaces[i]);
		}
		lumpLength = bspFile->fileHeader.lumps[LUMP_VERTS].length;
		verts_.reserve(lumpLength);
		for(int i = 0; i < lumpLength; i++) {
			verts_.push_back(bspFile->fileVerts[i]);
		}
		lumpLength = bspFile->fileHeader.lumps[LUMP_FACE_VERTS].length;
		faceVerts_.reserve(lumpLength);
		for(int i = 0; i < lumpLength; i++) {
			faceVerts_.push_back(bspFile->fileFaceVerts[i]);
		}
		lumpLength = bspFile->fileHeader.lumps[LUMP_FACES].length;
		for(int i = 0; i < lumpLength; i++) {
			faces_.push_back(bspFile->fileFaces[i]);
		}
		lumpLength = bspFile->fileHeader.lumps[LUMP_MATERIALS].length;
		materials_.reserve(lumpLength);
		for(int i = 0; i < lumpLength; i++) {
			materials_.push_back(bspFile->fileMaterials[i]);
		}
		lumpLength = bspFile->fileHeader.lumps[LUMP_LUMELS].length;
		lightmaps_.reserve(lumpLength);
		for(int i = 0; i < lumpLength; i++) {
			lightmaps_.push_back(bspFile->fileLightmaps[i]);
		}

		delete bspFile;

		renderFaces_.reserve(faces_.size());
		for(FileFace face : faces_) {
			RenderFace renderFace;

			std::vector<unsigned int> indices;
			indices.reserve(3 * (face.numVerts - 2));
			
			unsigned int firstVertIdx = face.firstVert;
			for(int i = 0; i < face.numVerts - 2; i++) {
				indices.push_back(firstVertIdx);
				indices.push_back(firstVertIdx + i + 1);
				indices.push_back(firstVertIdx + i + 2);
			}
			renderFace.SetPlane(&planes_[face.planeNum]);

			renderFace.SetIndices(indices);

			renderFaces_.push_back(renderFace);
		}

		glGenVertexArrays(1, &vao_);
		glGenBuffers(1, &vbo_);
		glGenBuffers(1, &ebo_);

		glBindVertexArray(vao_);

		if(buffer_.empty()) {
			buffer_.reserve(verts_.size() * 10);

			for(int i = 0; i < verts_.size(); i++) {
				buffer_.push_back(verts_[i].point[0]);
				buffer_.push_back(verts_[i].point[1]);
				buffer_.push_back(verts_[i].point[2]);

				buffer_.push_back(verts_[i].normal[0]);
				buffer_.push_back(verts_[i].normal[1]);
				buffer_.push_back(verts_[i].normal[2]);

				buffer_.push_back(verts_[i].surfaceUV[0]);
				buffer_.push_back(verts_[i].surfaceUV[1]);

				buffer_.push_back(verts_[i].lightMapUV[0]);
				buffer_.push_back(verts_[i].lightMapUV[1]);
			}
		}

		glBindBuffer(GL_ARRAY_BUFFER, vbo_);
		glBufferData(GL_ARRAY_BUFFER, buffer_.size() * sizeof(float), buffer_.data(), GL_STATIC_DRAW);

		constexpr unsigned int stride = 10 * sizeof(float);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, stride, (void*)(8 * sizeof(float)));
		glEnableVertexAttribArray(3);

		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, MAX_EBO * sizeof(unsigned int), nullptr, GL_DYNAMIC_DRAW);

		glBindVertexArray(0);

		gpuRelease_ = [this]() {
			if(vao_ != 0) {
				glDeleteVertexArrays(1, &vao_);
			}
			if(vbo_ != 0) {
				glDeleteBuffers(1, &vbo_);
			}
			if(vbo_ != 0) {
				glDeleteBuffers(1, &ebo_);
			}
		};

		if(vao_ == 0) {
			std::cerr << "Failed to upload geometry to GPU" << std::endl;
			return;
		}

		buffer_.clear();

		std::cout << "Successfully generated scene from BSP file" << std::endl;
	}

	void Scene::BindShader(const ShaderProgram& shaderProgram, const glm::mat4 &worldMatrix, const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix) {
		shaderProgram.Bind();

		glm::mat4 modelViewMatrix = viewMatrix * worldMatrix;

		glBindBuffer(GL_UNIFORM_BUFFER, shaderProgram.GetMatricesUbo());
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(worldMatrix));
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4)*1, sizeof(glm::mat4), glm::value_ptr(viewMatrix));
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4)*2, sizeof(glm::mat4), glm::value_ptr(modelViewMatrix));
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4)*3, sizeof(glm::mat4), glm::value_ptr(projectionMatrix));
	}

	void Scene::UnbindShader(const ShaderProgram& shaderProgram) {
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

        shaderProgram.Unbind();
	}

	void Scene::Update(float deltaTime) {
	}

	void Scene::CreateDefaltCamera() {
		camera_ = std::make_shared<Camera>();
		camera_->SetPerspective(60.f, 4.f/3.f, 1.f, 100000.f);

		camera_->SetRotation(Quaternion(Vec3f(0.f, 1.f, 0.f), -0.75 * M_PI));
		camera_->SetPosition(Vec3f(0.f, 100.f, -1000.f));
	}

	void Scene::DrawLeaf(int leafIdx) {
		FileLeaf *leaf = &leafs_[leafIdx];
		Vec3f extents(	leaf->maxBound[0] - leaf->minBound[0], 
						leaf->maxBound[1] - leaf->minBound[1], 
						leaf->maxBound[2] - leaf->minBound[2]	);
		
		unsigned int leafFace = leaf->firstLeafFace;
		for(int i = 0; i < leaf->numLeafFaces; i++) {
			RenderFace *renderFace = &renderFaces_[leafFaces_[leafFace]];
			renderFace->SetCurrentIndexCount(currentIndexCount_);
			renderFace->Draw(frameNum_, camera_->GetPosition());
			currentIndexCount_ = renderFace->GetCurrentIndexCount();
			leafFace++;
		}
	}

	void Scene::DrawWorldNodeRecursive(int nodeIdx) {
		nodesTraversed_++;
		FileNode *node = &nodes_[nodeIdx];
		Vec3f extents(	node->maxBound[0] - node->minBound[0], 
						node->maxBound[1] - node->minBound[1], 
						node->maxBound[2] - node->minBound[2]	);
		
		FilePlane *plane = &planes_[node->planeNum];
		float dot = (camera_->GetPosition()).Dot(Vec3f(plane->normal[0], plane->normal[1], plane->normal[2]));
		bool side = (dot - plane->dist) < 0;
		int childNodeIdx = node->children[!side];

		if(childNodeIdx < 0) {
			if(childNodeIdx < -1) {
				DrawLeaf(-(childNodeIdx + 1));
			}
		}
		else {
			DrawWorldNodeRecursive(childNodeIdx);
		}

		childNodeIdx = node->children[side];

		if(childNodeIdx < 0) {
			if(childNodeIdx < -1) {
				DrawLeaf(-(childNodeIdx + 1));
			}
		}
		else {
			DrawWorldNodeRecursive(childNodeIdx);
		}
	}

	void Scene::Draw() {
		frameNum_++;
		currentIndexCount_ = 0;
		nodesTraversed_ = 0;
		if(models_.empty()) {
			return;
		}


		camera_->UpdateMatrices();

		glBindVertexArray(vao_);

		FileModel *worldModel = &models_[0];
		DrawWorldNodeRecursive(worldModel->headNode);

		auto shaderProgram = ShaderLoader::LoadShader(ShaderType::DEFAULT);
		BindShader(*shaderProgram, glm::mat4{1.f} , camera_->GetViewMatrix(), camera_->GetProjectionMatrix());

		glDrawElements(GL_TRIANGLES, currentIndexCount_, GL_UNSIGNED_INT, (void*)0);
		glBindVertexArray(0);

		UnbindShader(*shaderProgram);

		//std::cout << frameNum_ << std::endl;
		//std::cout << currentIndexCount_ << std::endl;
		//std::cout << nodesTraversed_ << " number of nodes traversed" << std::endl;
	}
}