#include "LightTrace.hpp"

#define ON_PLANE_EPSILON 0.01

Trace::Trace(BspFile *bspFile) {
	fileNodes = bspFile->fileNodes;
	filePlanes = bspFile->filePlanes;
}

bool Trace::TraceLine(Vec3f startPos, Vec3f endPos) {
	this->startPos = startPos;
	this->endPos = endPos;

	return TraceLine_r(0);
}

bool Trace::TraceLine_r(int nodeIdx) {
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
		return TraceLine_r(node->children[0]);
	}
	if(startDist < ON_PLANE_EPSILON && endDist < ON_PLANE_EPSILON) {
		return TraceLine_r(node->children[1]);
	}

	int side = (startDist < 0);

	bool frontTrace = TraceLine_r(node->children[side]);

	if(frontTrace) {
		return frontTrace;
	}

	return TraceLine_r(node->children[!side]);
}