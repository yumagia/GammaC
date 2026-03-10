#include "Portal.hpp"

#include "GammaFile.hpp"

Portal::Portal(BspFile *bspFile) {
	this->bspFile = bspFile;
}

void Portal::CreateWindingFromNode(FileNode *node) {
	FilePlane *plane = &bspFile->filePlanes[node->planeNum];

	Vec3f v0, v1, v2, v3;
	// Planar mapping with the node's bounds
	switch(plane->type % 3) {
		case 1:
			v0 = Vec3f(	-(plane->normal[1] * node->maxBound[1] + plane->normal[2] * node->minBound[2]) / plane->normal[0],
						node->maxBound[1],
						node->minBound[2]);
			v1 = Vec3f(	-(plane->normal[1] * node->maxBound[1] + plane->normal[2] * node->maxBound[2]) / plane->normal[0],
						node->maxBound[1],
						node->maxBound[2]);
			v2 = Vec3f(	-(plane->normal[1] * node->minBound[1] + plane->normal[2] * node->maxBound[2]) / plane->normal[0],
						node->minBound[1],
						node->maxBound[2]);
			v3 = Vec3f(	-(plane->normal[1] * node->minBound[1] + plane->normal[2] * node->minBound[2]) / plane->normal[0],
						node->minBound[1],
						node->minBound[2]);
			break;
		case 2:
			v0 = Vec3f(	node->minBound[0],
						-(plane->normal[0] * node->minBound[0] + plane->normal[2] * node->maxBound[2]) / plane->normal[1],
						node->maxBound[2]);
			v1 = Vec3f(	node->maxBound[0],
						-(plane->normal[0] * node->maxBound[0] + plane->normal[2] * node->maxBound[2]) / plane->normal[1],
						node->maxBound[2]);
			v2 = Vec3f(	node->maxBound[0],
						-(plane->normal[0] * node->maxBound[0] + plane->normal[2] * node->minBound[2]) / plane->normal[1],
						node->minBound[2]);
			v3 = Vec3f(	node->minBound[0],
						-(plane->normal[0] * node->minBound[0] + plane->normal[2] * node->minBound[2]) / plane->normal[1],
						node->minBound[2]);
			break;
		case 3:
			v0 = Vec3f(	node->maxBound[0],
						node->maxBound[1],
						-(plane->normal[0] * node->maxBound[0] + plane->normal[1] * node->maxBound[1]) / plane->normal[2]);
			v1 = Vec3f(	node->minBound[0],
						node->maxBound[1],
						-(plane->normal[0] * node->minBound[0] + plane->normal[1] * node->maxBound[1]) / plane->normal[2]);
			v2 = Vec3f(	node->minBound[0],
						node->minBound[1],
						-(plane->normal[0] * node->minBound[0] + plane->normal[1] * node->minBound[1]) / plane->normal[2]);
			v3 = Vec3f(	node->maxBound[0],
						node->minBound[1],
						-(plane->normal[0] * node->maxBound[0] + plane->normal[1] * node->minBound[1]) / plane->normal[2]);
			break;
	}

	winding.push_back(v0);
	winding.push_back(v1);
	winding.push_back(v2);
	winding.push_back(v3);
}