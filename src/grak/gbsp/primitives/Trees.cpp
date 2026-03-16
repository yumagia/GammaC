#include "../Bsp.hpp"
#include "GammaFile.hpp"

#include <iostream>

extern	BspPlane	mapPlanes[MAX_MAP_PLANES];
extern	int			numMapPlanes;

void FreeTreePortals_r(BspNode *node) {
	if(!node->isLeaf) {
		FreeTreePortals_r(node->front);
		FreeTreePortals_r(node->back);
	}

	BspPortal *curr = node->portals;
	while(curr) {
		int side = curr->GetNextNodeSide(node);
		BspPortal *next = curr->GetNext(side);

		curr->RemoveFromNode(curr->GetNextNode(!side));
		delete curr;

		curr = next;
	}

	node->portals = NULL;
}

void FreeTree_r(BspNode *node) {
    std::shared_ptr<BspFace> f;

    if(!node->isLeaf) {
        FreeTree_r(node->back);
        FreeTree_r(node->front);
    }
    else {
        while(!node->faces.empty()) {
            f = node->faces.back();
            node->faces.pop_back();

			f.reset();
        }
    }

    delete node;
}

void FreeTree(BspNode *node) {
	FreeTreePortals_r(node);
	FreeTree_r(node);
}

void PrintTree(BspNode *node, int depth) {
	for(int i = 0; i < depth; i++) {
		std::cout << "   |";
	}

	if(node->isLeaf) {
		if(node->faces.empty()) {
			std::cout << "___ SOLID" << std::endl;
		}
		else {
			std::cout << "___ " << node->faces.size() << " face(s)" << std::endl;
		}
		return;
	}

	BspPlane *plane;
	plane = &mapPlanes[node->planeNum];

	std::cout << "___ (" 
		<< plane->normal.x << ", " 
		<< plane->normal.y << ", " 
		<< plane->normal.z << ") " 
		<< plane->dist << std::endl;
	PrintTree(node->back, depth + 1);
	PrintTree(node->front, depth + 1);

	return;
}