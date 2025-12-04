#include "Bsp.hpp"
#include "GammaFile.h"

#include <iostream>

extern	BspPlane	mapPlanes[MAX_MAP_PLANES];
extern	int			numMapPlanes;

void FreeTree(BspNode *node) {
    std::shared_ptr<BspFace> f;

    if(!node->isLeaf) {
        FreeTree(node->back);
        FreeTree(node->front);
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