#include "PortalGenerator.hpp"


void PortalGenerator::GeneratePortals(BspFile *bspFile) {
	this->bspFile = bspFile;
	FileNode *headNode = &bspFile->fileNodes[bspFile->fileModels[0].headNode];

	GeneratePortals_r(headNode);
}

void PortalGenerator::GeneratePortals_r(FileNode *node) {
	Portal *superPortal = new Portal(bspFile);

	superPortal->CreateWindingFromNode(node);
}