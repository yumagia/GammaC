#include "GammaFile.hpp"

#include "PortalWriter.hpp"
#include <iostream>

extern	BspPlane	mapPlanes[MAX_MAP_PLANES];
extern	int			numMapPlanes;

void PortalWriter::GeneratePortals_r(BspNode *node) {

	if(node->isLeaf) {
		return;
	}

	CreateNodePortals(node);
	SplitNodePortals(node);

	GeneratePortals_r(node->front);
	GeneratePortals_r(node->back);
}

void PortalWriter::CreateNodePortals(BspNode *node) {
	std::shared_ptr<BspPortal> portal = std::make_shared<BspPortal>();

	portal->CreateWindingFromNode(node);

	if(portal->WindingValid()) {
		AddPortalToNodes(portal, node->front, node->back);
	}
}

void PortalWriter::SplitNodePortals(BspNode *node) {
	
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

void PortalWriter::AddPortalToNodes(std::shared_ptr<BspPortal> portal, BspNode *front, BspNode *back) {
	portal->AddToNodes(front, back);

	front->portals = back->portals = portal;
}

// Leaves under a given node will all be given this number
void PortalWriter::FillLeafNumbers_r(BspNode *node, int num) {
	if(node->isLeaf) {
		if(node->contents & CONTENTS_SOLID) {
			node->cluster = -1;
		}
		else {
			node->cluster = num;
		}

		return;
	}

	node->cluster = num;
	FillLeafNumbers_r(node->front, num);
	FillLeafNumbers_r(node->back, num);
}

// Generate the clusters for vis, and link them to the leaves
void PortalWriter::NumberLeafs_r(BspNode *node) {
	if(!node->isLeaf && !node->detailSeperator) {
		NumberLeafs_r(node->front);
		NumberLeafs_r(node->back);
		return;
	}

	// Now we're either in a leaf or detail cluster

	if(node->contents & CONTENTS_SOLID) {	// Not traversable by view
		node->cluster = -1;
		return;
	}

	FillLeafNumbers_r(node, numVisClusters);
	numVisClusters++;

	// Count the portals
	std::shared_ptr<BspPortal> curr = node->portals;
	while(curr) {
		if(curr->GetNextNode(0) == node) {		// Only write them out from the first leaf
			if(curr->VisFlood()) {
				numVisPortals++;
			}
			curr = curr->GetNext(0);
		}
		else {
			curr = curr->GetNext(1);
		}
	}
}

void PortalWriter::WritePortals(BspNode *node) {
	numVisClusters = numVisPortals = 0;
	
	GeneratePortals_r(node);

	//NumberLeafs_r(node);
}