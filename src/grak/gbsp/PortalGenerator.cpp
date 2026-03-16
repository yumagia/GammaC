#include "PortalGenerator.hpp"

void PortalGenerator::GeneratePortals_r(BspNode *node) {

	if(node->isLeaf) {
		return;
	}

	CreateNodePortals(node);
}

void PortalGenerator::CreateNodePortals(BspNode *node) {
	std::shared_ptr<BspPortal> portal = std::make_shared<BspPortal>();

	portal->CreateWindingFromNode(node);

	if(portal->WindingValid()) {
		AddPortalToNodes(portal, node->front, node->back);
	}
}

void PortalGenerator::AddPortalToNodes(std::shared_ptr<BspPortal> portal, BspNode *front, BspNode *back) {
	portal->AddToNodes(front, back);

	front->portals = back->portals = portal;
}

void PortalGenerator::GeneratePortals(BspNode *node) {
	GeneratePortals_r(node);
}