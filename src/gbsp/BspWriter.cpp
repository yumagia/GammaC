#include <iostream>

#include "BspWriter.hpp"

#include "Bsp.hpp"
#include "GammaFile.h"

extern	int			numMapPlanes;
extern int		numMapVerts;
extern int		numMapFaceVerts;


FileWriter::FileWriter() {
	numModels = 1;
	numEntities = 0;
	numPlanes = 0;
	numNodes = 0;
	numLeafs = 1;
	numVerts = 1;
	numFaceVerts = 1;
	numFaces = 0;
}

void FileWriter::BeginBspFile() {
	std::cout << "Initializing BSP file" << std::endl;

	numMapPlanes = 0;
	numMapVerts = 0;
	numMapFaceVerts = 0;
}


void FileWriter::WriteLevel() {
	std::cout << "--- WriteLevel ---" << std::endl;
}

void FileWriter::AddWorldModel(BspModel *model) {
	startLeaf = numLeafs;
	startFaceVert = numFaceVerts;
	startFace = numFaces;
	
	bspFile->fileModels[0].firstFace = numFaces;
	
	bspFile->fileModels[0].minBound[0] = model->bounds.min.x;
	bspFile->fileModels[0].minBound[1] = model->bounds.min.y;
	bspFile->fileModels[0].minBound[2] = model->bounds.min.z;
	bspFile->fileModels[0].maxBound[0] = model->bounds.max.x;
	bspFile->fileModels[0].maxBound[1] = model->bounds.max.y;
	bspFile->fileModels[0].maxBound[2] = model->bounds.max.z;

	bspFile->fileModels[0].headNode = EmitTree(model->root);
}

void FileWriter::EmitLeaf(BspNode *node) {
	FileLeaf *emittedLeaf;

	if(numLeafs >= MAX_MAP_LEAFS) {
		std::cerr << "Reached MAX_MAP_LEAFS: " << MAX_MAP_LEAFS << std::endl;
	}

	emittedLeaf = &bspFile->fileLeafs[numLeafs];
	numLeafs++;

	emittedLeaf->minBound[0] = node->bounds.min.x;
	emittedLeaf->minBound[1] = node->bounds.min.y;
	emittedLeaf->minBound[2] = node->bounds.min.z;
	emittedLeaf->maxBound[0] = node->bounds.max.x;
	emittedLeaf->maxBound[1] = node->bounds.max.y;
	emittedLeaf->maxBound[2] = node->bounds.max.z;

	if(node->faces.empty()) {
		emittedLeaf->numLeafFaces = 0;
		return;
	}

	emittedLeaf->firstLeafFace = numLeafFaces;

	for(BspFace *face : node->faces) {
		int faceNum = face->outputNumber;
		if(numLeafFaces >= MAX_MAP_LEAF_FACES) {
			std::cerr << "Reached MAX_MAP_LEAF_FACES: " << MAX_MAP_LEAF_FACES << std::endl;
		}
		bspFile->fileLeafFaces[numLeafFaces] = faceNum;
		numLeafFaces++;
	}

	emittedLeaf->numLeafFaces = numLeafFaces - emittedLeaf->firstLeafFace;
}

void FileWriter::EmitFace(BspFace *face) {
	FileFace *emittedFace;
	
	face->outputNumber = numFaces;

	if(numFaces >= MAX_MAP_FACES) {
		std::cerr << "Reached MAX_MAP_FACES: " << MAX_MAP_FACES << std::endl;
	}

	emittedFace = &bspFile->fileFaces[numFaces];
	numFaces++;

	emittedFace->planeNum = face->planeNum;
	
	emittedFace->firstVert = numFaceVerts;
	int numVerts = face->vertIndices.size();
	emittedFace->numVerts = numVerts;
	//TODO: HASH YOUR VERTS
}

int FileWriter::EmitTree(BspNode *node) {
	
	if(node->isLeaf) {
		EmitLeaf(node);
		return -numLeafs;
	}

	if(numNodes >= MAX_MAP_NODES) {
		std::cerr << "Reached MAX_MAP_NODES: " << MAX_MAP_NODES << std::endl;
	}
	
	FileNode *emittedNode;
	emittedNode = &bspFile->fileNodes[numNodes];
	numNodes++;

	emittedNode->minBound[0] = node->bounds.min.x;
	emittedNode->minBound[1] = node->bounds.min.y;
	emittedNode->minBound[2] = node->bounds.min.z;
	emittedNode->maxBound[0] = node->bounds.max.x;
	emittedNode->maxBound[1] = node->bounds.max.y;
	emittedNode->maxBound[2] = node->bounds.max.z;

	emittedNode->planeNum = node->planeNum;
	numPlanes++;

	for(BspFace *face : node->faces) {
		EmitFace(face);
	}

	if(node->back->isLeaf) {
		emittedNode->children[0] = -(numLeafs + 1);
		EmitLeaf(node->back);
	}
	else {
		emittedNode->children[0] = numNodes;
		EmitTree(node->back);
	}

	if(node->front->isLeaf) {
		emittedNode->children[1] = -(numLeafs + 1);
		EmitLeaf(node->front);
	}
	else {
		emittedNode->children[1] = numNodes;
		EmitTree(node->front);
	}

	return emittedNode - bspFile->fileNodes;
}

