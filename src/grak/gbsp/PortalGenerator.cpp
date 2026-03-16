#include "PortalGenerator.hpp"
#include <iostream>

extern	BspPlane	mapPlanes[MAX_MAP_PLANES];
extern	int			numMapPlanes;

void PortalGenerator::GeneratePortals_r(BspNode *node) {

	if(node->isLeaf) {
		return;
	}

	CreateNodePortals(node);
	SplitNodePortals(node);

	GeneratePortals_r(node->front);
	GeneratePortals_r(node->back);
}

void PortalGenerator::CreateNodePortals(BspNode *node) {
	std::shared_ptr<BspPortal> portal = std::make_shared<BspPortal>();

	portal->CreateWindingFromNode(node);

	if(portal->WindingValid()) {
		AddPortalToNodes(portal, node->front, node->back);
	}
}

void PortalGenerator::SplitNodePortals(BspNode *node) {
	
	std::shared_ptr<BspPortal> curr = node->portals;
	while(curr) {
		int side = curr->GetNextNodeSide(node);
		
		std::shared_ptr<BspPortal> next = curr->GetNext(side);

		BspNode *otherNode = curr->GetNextNode(!side);
		curr->RemoveFromNode(curr->GetNextNode(0));
		curr->RemoveFromNode(curr->GetNextNode(1));

		std::shared_ptr<BspPortal> frontPortal, backPortal;
		switch (curr->Split(&mapPlanes[node->planeNum], frontPortal, backPortal)) {
			case SplitPortalResult::COPLANAR:
				break;
			case SplitPortalResult::BACK:
				if(side == 0) {
					AddPortalToNodes(curr, node->back, otherNode);
				}
				else {
					AddPortalToNodes(curr, otherNode, node->back);
				}
				break;
			case SplitPortalResult::FRONT:
				if(side == 0) {
					AddPortalToNodes(curr, node->front, otherNode);
				}
				else {
					AddPortalToNodes(curr, otherNode, node->front);
				}
				break;
			case SplitPortalResult::SPLIT:
				if(side == 0) {
					AddPortalToNodes(frontPortal, node->front, otherNode);
					AddPortalToNodes(backPortal, node->back, otherNode);
				}
				else {
					AddPortalToNodes(frontPortal, otherNode, node->front);
					AddPortalToNodes(backPortal, otherNode, node->back);
				}
				break;
		}

		curr = next;
	}

	node->portals = NULL;
}

void PortalGenerator::AddPortalToNodes(std::shared_ptr<BspPortal> portal, BspNode *front, BspNode *back) {
	portal->AddToNodes(front, back);

	front->portals = back->portals = portal;
}

void PortalGenerator::GeneratePortals(BspNode *node) {
	GeneratePortals_r(node);
}