#include "Portal.hpp"
#include "GammaFile.hpp"

#include <iostream>

#define PLANE_EPSILON 0.01

enum {
	FRONT,
	BACK,
	SPLIT,
	ERROR
};

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

	this->plane.normal.x = plane->normal[0];
	this->plane.normal.y = plane->normal[1];
	this->plane.normal.z = plane->normal[2];
	this->plane.dist = plane->dist;
}

Vec3f Portal::SegmentPlaneIntersection(Vec3f p1, Vec3f p2, Plane *plane) {
	float d = -(plane->normal.Dot(p1) - plane->dist) / plane->normal.Dot(p2 - p1);
	return p1 + d * (p2 - p1);
}

int Portal::Split(Plane *plane, Portal &front, Portal &back) {
	// First check if its coplanar
	int count = 0;
	for(Vec3f vertex : winding) {
		if(fabs(plane->normal.Dot(vertex) - plane->dist) <= PLANE_EPSILON) {
			count++;
		}
		else {
			break;
		}
	}

	if(count == winding.size()) {
		float dot = plane->normal.Dot(this->plane.normal);
		if(dot > PLANE_EPSILON) {
			return FRONT;
		}
		if(dot < -PLANE_EPSILON) {
			return BACK;
		}

		std::cerr << "WARNING: Potentially tiny portal" << std::endl;
	}

	// Split it
	// This is a lot like the split routine in BSP
	front = back = NULL;

	int numVerts = winding.size();
	std::vector<Vec3f> frontVerts, backVerts;
	Vec3f prev = winding[numVerts - 1];
	int prevDot = plane->normal.Dot(prev) - plane->dist;
	for(int n = 0; n < numVerts; n++) {
		Vec3f curr = winding[n];
		int currDot = plane->normal.Dot(curr) - plane->dist;
		if(currDot > PLANE_EPSILON) {
			if(prevDot < -PLANE_EPSILON) {
				// Current edge intersects plane,
				// So output the intersection point
				Vec3f intersection = SegmentPlaneIntersection(curr, prev, plane);
				frontVerts.push_back(intersection);
				backVerts.push_back(intersection);
			}

			// Output b to front side in all three cases
			frontVerts.push_back(curr);
		}
		else if(currDot < -PLANE_EPSILON) {
			if(prevDot > PLANE_EPSILON) {
				// Also an intersection
				Vec3f intersection = SegmentPlaneIntersection(curr, prev, plane);
				frontVerts.push_back(intersection);
				backVerts.push_back(intersection);
			}
			else if(prevDot > -PLANE_EPSILON) {	// Trailing vertex of edge is "on" plane
				backVerts.push_back(prev);

			}

			// Output b to back side in all three cases
			backVerts.push_back(curr);
		} else {
			// Leading vertex of edge is on the plane,
			// So output it to the front side
			frontVerts.push_back(curr);
			if(prevDot < -PLANE_EPSILON) {
				backVerts.push_back(curr);
			}
		}

		// Set the trailing edge
		prev = curr; 
		prevDot = currDot;
	}

	if(frontVerts.size() == 0 || backVerts.size() == 0) {
		for(Vec3f vertex : winding) {
			float dist = plane->normal.Dot(vertex) - plane->dist;
			if(dist > PLANE_EPSILON) {
				return FRONT;
			}
			else if(dist < -PLANE_EPSILON) {
				return BACK;
			}
		}
	}
	else {
		return SPLIT;
	}

	return ERROR;
}