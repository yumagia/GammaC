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

    filePlanes[numPlanes].normal[0] = node->plane->normal.x;
    filePlanes[numPlanes].normal[1] = node->plane->normal.y;
    filePlanes[numPlanes].normal[2] = node->plane->normal.z;

    numPlanes++;

    emittedNode->planeNum = node->plane;
    

}