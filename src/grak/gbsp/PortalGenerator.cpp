#include "PortalGenerator.hpp"

void PortalGenerator::GeneratePortals_r(BspNode *node) {

	if(node->isLeaf) {
		return;
	}

	BspPortal *portal = new BspPortal();

	portal->CreateWindingFromNode(node);

	if(portal->WindingValid()) {
		AddPortalToNodes(portal, node->front, node->back);
	}
}

void PortalGenerator::AddPortalToNodes(BspPortal *portal, BspNode *front, BspNode *back) {
	portal->AddToNodes(front, back);

	front->portals = back->portals = portal;
}

void PortalGenerator::GeneratePortals(BspNode *node) {
	GeneratePortals_r(node);
}