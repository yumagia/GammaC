#include "GammaFile.hpp"
#include "Bsp.hpp"

#include <memory>
#include <iostream>

extern	BspPlane	mapPlanes[MAX_MAP_PLANES];
extern	int			numMapPlanes;

#define PLANE_EPSILON 0.01

BspPortal::BspPortal() {
	nodes[0] = nodes[1] = NULL;
}

void BspPortal::CreateWindingFromNode(BspNode *node) {
	BspPlane *plane = &mapPlanes[node->planeNum];

	Vec3f *maxBound = &node->bounds.max;
	Vec3f *minBound = &node->bounds.min;

	Vec3f v0, v1, v2, v3;
	// Planar mapping with the node's bounds to create a "superportal" winding
	switch(plane->type % 3) {
		case 1:
			v0 = Vec3f(	-(plane->normal.y * maxBound->y + plane->normal.z * minBound->z) / plane->normal.x,
						maxBound->y,
						maxBound->z);
			v1 = Vec3f(	-(plane->normal.y * maxBound->y + plane->normal.z * maxBound->z) / plane->normal.x,
						maxBound->y,
						maxBound->z);
			v2 = Vec3f(	-(plane->normal.y * minBound->y + plane->normal.z * maxBound->z) / plane->normal.x,
						minBound->y,
						maxBound->z);
			v3 = Vec3f(	-(plane->normal.y * minBound->y + plane->normal.z * minBound->z) / plane->normal.x,
						minBound->y,
						minBound->z);
			break;
		case 2:
			v0 = Vec3f(	minBound->x,
						-(plane->normal.x * minBound->x + plane->normal.z * maxBound->z) / plane->normal.y,
						maxBound->z);
			v1 = Vec3f(	maxBound->x,
						-(plane->normal.x * maxBound->x + plane->normal.z * maxBound->z) / plane->normal.y,
						maxBound->z);
			v2 = Vec3f(	maxBound->x,
						-(plane->normal.x * maxBound->x + plane->normal.z * minBound->z) / plane->normal.y,
						minBound->z);
			v3 = Vec3f(	minBound->x,
						-(plane->normal.x * minBound->x + plane->normal.z * minBound->z) / plane->normal.y,
						minBound->z);
			break;
		case 3:
			v0 = Vec3f(	maxBound->x,
						maxBound->y,
						-(plane->normal.x * maxBound->x + plane->normal.y * maxBound->y) / plane->normal.z);
			v1 = Vec3f(	minBound->x,
						maxBound->y,
						-(plane->normal.x * minBound->x + plane->normal.y * maxBound->y) / plane->normal.z);
			v2 = Vec3f(	minBound->x,
						minBound->y,
						-(plane->normal.x * minBound->x + plane->normal.y * minBound->y) / plane->normal.z);
			v3 = Vec3f(	maxBound->x,
						minBound->y,
						-(plane->normal.x * maxBound->x + plane->normal.y * minBound->y) / plane->normal.z);
			break;
	}

	winding.push_back(v0);
	winding.push_back(v1);
	winding.push_back(v2);
	winding.push_back(v3);

	this->plane.normal.x = plane->normal.x;
	this->plane.normal.y = plane->normal.y;
	this->plane.normal.z = plane->normal.z;
	this->plane.dist = plane->dist;

	BspNode *parent = node->parent;
	while(parent != NULL) {
		Chop(&mapPlanes[parent->planeNum]);

		parent = parent->parent;
	}
}

SplitPortalResult BspPortal::Split(BspPlane *plane, std::shared_ptr<BspPortal> front, std::shared_ptr<BspPortal> back) {
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
			return SplitPortalResult::FRONT;
		}
		if(dot < -PLANE_EPSILON) {
			return SplitPortalResult::BACK;
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
				Vec3f intersection = SegmentPlaneIntersection(curr, prev, *plane);
				frontVerts.push_back(intersection);
				backVerts.push_back(intersection);
			}

			// Output b to front side in all three cases
			frontVerts.push_back(curr);
		}
		else if(currDot < -PLANE_EPSILON) {
			if(prevDot > PLANE_EPSILON) {
				// Also an intersection
				Vec3f intersection = SegmentPlaneIntersection(curr, prev, *plane);
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
				return SplitPortalResult::FRONT;
			}
			else if(dist < -PLANE_EPSILON) {
				return SplitPortalResult::BACK;
			}
		}
	}
	else {
		return SplitPortalResult::SPLIT;
	}

	return SplitPortalResult::COPLANAR;
}

bool BspPortal::WindingValid() {
	if(winding.size() <= 2) {
		return false;
	}
	
	return true;
}

// Sutherland-Hodgman
void BspPortal::Chop(BspPlane *plane) {
	if(!WindingValid()) {
		return;
	}

	std::vector<Vec3f> choppedWinding;

	float prevDot, currDot;
	Vec3f prev = winding[winding.size()];
	prevDot = plane->normal.Dot(prev) - plane->dist;
	for(Vec3f curr : winding) {
		currDot = plane->normal.Dot(curr) - plane->dist;

		if(currDot > PLANE_EPSILON) {
			if(prevDot < -PLANE_EPSILON) {
				choppedWinding.push_back(SegmentPlaneIntersection(curr, prev, *plane));
			}

			choppedWinding.push_back(curr);
		}
		else if(currDot < -PLANE_EPSILON) {
			choppedWinding.push_back(curr);
		}
		else if(prevDot > PLANE_EPSILON) {
			choppedWinding.push_back(SegmentPlaneIntersection(curr, prev, *plane));
		}

		prevDot = currDot;
		prev = curr;
	}

	winding = choppedWinding;
}

void BspPortal::AddToNodes(BspNode *front, BspNode *back) {
	if(nodes[0] || nodes[1]) {
		std::cerr << "BspPortal error: Portal already included" << std::endl;
	}

	nodes[0] = front;
	next[0] = front->portals;

	nodes[1] = back;
	next[1] = back->portals;
}

// Returns 0 upon success
int BspPortal::RemoveFromNode(BspNode *node) {
	std::shared_ptr<BspPortal> curr;
	std::shared_ptr<BspPortal> *portalPointer = &node->portals;
	while(true) {
		curr = *portalPointer;
		if(!curr) {
			std::cerr << "BspPortal Error: Portal does not exist in leaf" << std::endl;
			return 1;
		}

		if(curr.get() == this) {
			break;
		}

		if(curr->GetNextNode(0) == node) {
			portalPointer = &curr->next[0];
		}
		else if(curr->GetNextNode(1) == node) {
			portalPointer = &curr->next[1];
		}
		else {
			std::cerr << "BspPortal Error: Portal not bounding leaf" << std::endl;
			return 1;
		}
	}

	if(nodes[0] == node) {
		*portalPointer = next[0];
		nodes[0] = NULL;
	}
	else if(nodes[1] == node) {
		*portalPointer = next[1];
		nodes[1] = NULL;
	}

	return 0;
}

int BspPortal::GetNextNodeSide(BspNode *node) {
	return (nodes[1] == node);
}

std::shared_ptr<BspPortal> BspPortal::GetNext(int side) {
	return next[side];
}

BspNode *BspPortal::GetNextNode(int side) {
	return nodes[side];
}
