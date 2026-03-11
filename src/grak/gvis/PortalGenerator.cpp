#include "PortalGenerator.hpp"

void PortalGenerator::GenerateWorldBackTraces() {
	int headNodeIdx = bspFile->fileModels[0].headNode;

	nodeParents.reserve(bspFile->fileHeader.lumps[LUMP_NODES].length);
	GenerateWorldBackTraces_r(headNodeIdx, -1);
}

void PortalGenerator::GenerateWorldBackTraces_r(int nodeIdx, int parentIdx) {
	nodeParents[nodeIdx] = parentIdx;

	
}

void PortalGenerator::GeneratePortals_r(FileNode *node, FileNode *parent) {
	Portal *superPortal = new Portal(bspFile);

	superPortal->CreateWindingFromNode(node);
}

void PortalGenerator::GeneratePortals(BspFile *bspFile) {
	this->bspFile = bspFile;

	FileNode *headNode = &bspFile->fileNodes[bspFile->fileModels[0].headNode];
	GenerateWorldBackTraces();
	GeneratePortals_r(headNode, NULL);
}