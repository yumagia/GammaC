#include <iostream>

#include "LevelWriter.hpp"

#include "Bsp.hpp"
#include "GammaFile.h"

LevelWriter::LevelWriter() {
	numModels = 1;
	numEntities = 0;
	numPlanes = 0;
	numNodes = 0;
	numLeafs = 1;
	numVerts = 1;
	numEdges = 1;
	numFaces = 0;
}


void LevelWriter::WriteLevel() {
	std::cout << "--- WriteLevel ---" << std::endl;
}

void LevelWriter::AddWorldModel(BspModel *model) {
	startLeaf = numLeafs;
	startEdge = numEdges;
	startFace = numFaces;
	
	fileModels[0].firstFace = numFaces;
	
	fileModels[0].minBound[0] = model->bounds.min.x;
	fileModels[0].minBound[1] = model->bounds.min.y;
	fileModels[0].minBound[2] = model->bounds.min.z;
	fileModels[0].maxBound[0] = model->bounds.max.x;
	fileModels[0].maxBound[1] = model->bounds.max.y;
	fileModels[0].maxBound[2] = model->bounds.max.z;

	fileModels[0].headNode = EmitTree(model->root);
}

void LevelWriter::EmitLeaf(BspNode *node) {
	FileLeaf *emittedLeaf;

	if(numLeafs >= MAX_MAP_LEAFS) {
		std::cerr << "Reached MAX_MAP_LEAFS: " << MAX_MAP_LEAFS << std::endl;
	}

	emittedLeaf = &fileLeafs[numLeafs];
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
		fileLeafFaces[numLeafFaces] = faceNum;
		numLeafFaces++;
	}

	emittedLeaf->numLeafFaces = numLeafFaces - emittedLeaf->firstLeafFace;
}

int LevelWriter::EmitTree(BspNode *node) {
	
	if(node->isLeaf) {
		EmitLeaf(node);
		return -numLeafs;
	}

	if(numNodes >= MAX_MAP_NODES) {
		std::cerr << "Reached MAX_MAP_NODES: " << MAX_MAP_NODES << std::endl;
	}
	
	FileNode *emittedNode;
	emittedNode = &fileNodes[numNodes];
	numNodes++;

	emittedNode->minBound[0] = node->bounds.min.x;
	emittedNode->minBound[1] = node->bounds.min.y;
	emittedNode->minBound[2] = node->bounds.min.z;
	emittedNode->maxBound[0] = node->bounds.max.x;
	emittedNode->maxBound[1] = node->bounds.max.y;
	emittedNode->maxBound[2] = node->bounds.max.z;


	// TODO: Hash these...
	filePlanes[numPlanes].normal[0] = node->plane->normal.x;
	filePlanes[numPlanes].normal[1] = node->plane->normal.y;
	filePlanes[numPlanes].normal[2] = node->plane->normal.z;
	filePlanes[numPlanes].dist = node->plane->dist;

	emittedNode->planeNum = numPlanes;
	numPlanes++;

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

	return emittedNode - fileNodes;
}

