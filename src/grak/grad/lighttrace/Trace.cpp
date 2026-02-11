#include "Trace.hpp"

#define ON_PLANE_EPSILON	0.1f

#include <iostream>

Trace::Trace(BspFile *bspFile) {
	fileNodes = bspFile->fileNodes;
	fileLeafs = bspFile->fileLeafs;
	filePlanes = bspFile->filePlanes;
}


bool Trace::TraceLine_r(int nodeIdx, Vec3f startPos, Vec3f endPos) {
	if(nodeIdx < 0) {
		if(nodeIdx == -1) {
			this->hitPoint = startPos;
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
		return TraceLine_r(node->children[0], startPos, endPos);
	}
	if(startDist < ON_PLANE_EPSILON && endDist < ON_PLANE_EPSILON) {
		return TraceLine_r(node->children[1], startPos, endPos);
	}

	int side = (startDist < 0);
	float d = startDist / (startDist - endDist);
	Vec3f intersect = startPos + d * (endPos - startPos);

	if(TraceLine_r(node->children[side], startPos, intersect) ) {
		return true;
	}

	this->hitNodeIdx = nodeIdx;

	return TraceLine_r(node->children[!side], intersect, endPos);
}

bool Trace::TraceLine(Vec3f startPos, Vec3f endPos) {
	hitNodeIdx = -1;

	return TraceLine_r(0, startPos, endPos);
}

bool Trace::LineStab_r(int nodeIdx) {
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
		return LineStab_r(node->children[0]);
	}
	if(startDist < ON_PLANE_EPSILON && endDist < ON_PLANE_EPSILON) {
		return LineStab_r(node->children[1]);
	}

	int side = (startDist < 0);

	if(LineStab_r(node->children[side])) {
		return true;
	}

	return LineStab_r(node->children[!side]);
}

bool Trace::LineStab(Vec3f startPos, Vec3f endPos) {
	this->startPos = startPos;
	this->endPos = endPos;

	return LineStab_r(0);
}