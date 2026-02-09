#include "Trace.hpp"

#define ON_PLANE_EPSILON	0.01

#include <iostream>

Trace::Trace(BspFile *bspFile) {
	fileNodes = bspFile->fileNodes;
	fileLeafs = bspFile->fileLeafs;
	filePlanes = bspFile->filePlanes;
}

bool Trace::FastTraceLine(Vec3f startPos, Vec3f endPos) {
	this->startPos = startPos;
	this->endPos = endPos;
	hitNodeIdx = -1;

	return FastTraceLine_r(0, 0);
}

bool Trace::FastTraceLine_r(int parentIdx, int nodeIdx) {
	if(nodeIdx < 0) {
		if(nodeIdx == -1) {
			return true;
		}

		return false;
	}

	FileNode *node = &fileNodes[nodeIdx];
	FilePlane *plane = &filePlanes[node->planeNum];
	Vec3f normal = Vec3f(plane->normal[0], plane->normal[1], plane->normal[2]);
	float startDist = normal.Dot(startPos) - plane->dist;
	float endDist = normal.Dot(endPos) - plane->dist;

	if(startDist > -ON_PLANE_EPSILON && endDist > -ON_PLANE_EPSILON) {
		return FastTraceLine_r(nodeIdx, node->children[0]);
	}
	if(startDist < ON_PLANE_EPSILON && endDist < ON_PLANE_EPSILON) {
		return FastTraceLine_r(nodeIdx, node->children[1]);
	}

	int side = (startDist < 0);

	if(FastTraceLine_r(nodeIdx, node->children[side])) {
		return true;
	}

	hitNodeIdx = nodeIdx;

	return FastTraceLine_r(nodeIdx, node->children[!side]);
}