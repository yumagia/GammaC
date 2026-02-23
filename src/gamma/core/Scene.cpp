#include "Scene.hpp"

#include "FileReader.hpp"

#include <iostream>

namespace GammaEngine {
	Scene::Scene() {
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

			renderFace.SetIndices(indices);

			renderFaces_.push_back(renderFace);
		}

		glGenVertexArrays(1, &vao_);
		glGenBuffers(1, &vbo_);

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

		glBindVertexArray(0);

		gpuRelease_ = [this]() {
			if(vao_ != 0) {
				glDeleteVertexArrays(1, &vao_);
			}
			if(vbo_ != 0) {
				glDeleteBuffers(1, &vbo_);
			}
		};

		if(vao_ == 0) {
			std::cerr << "Failed to upload geometry to GPU" << std::endl;
			return;
		}

		buffer_.clear();

		std::cout << "Successfully generated scene from BSP file" << std::endl;
	}

	void Scene::Update(float deltaTime) {
	}

	void Scene::DrawLeaf(int leafIdx) {
		FileLeaf *leaf = &leafs_[leafIdx];
		Vec3f extents(	leaf->maxBound[0] - leaf->minBound[0], 
						leaf->maxBound[1] - leaf->minBound[1], 
						leaf->maxBound[2] - leaf->minBound[2]	);
		
		unsigned int leafFace = leaf->firstLeafFace;
		for(int i = 0; i < leaf->numLeafFaces; i++) {
			renderFaces_[leafFaces_[leafFace]].Draw();
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
		float dot = (view_.position_).Dot(Vec3f(plane->normal[0], plane->normal[1], plane->normal[2]));
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
		nodesTraversed_ = 0;
		view_ = camera_.GetTransform();

		if(models_.empty()) {
			return;
		}

		FileModel *worldModel = &models_[0];

		DrawWorldNodeRecursive(worldModel->headNode);

		//std::cout << nodesTraversed_ << " number of nodes traversed" << std::endl;
	}
}