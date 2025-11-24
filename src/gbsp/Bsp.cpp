#include "GammaFile.h"
#include "Bsp.hpp"
#include "Math.hpp"

#include <vector>
#include <iostream>

#define PLANE_EPSILON 0.01
#define FLOAT_MAX 999999999
#define SPLIT_BALANCE 0.8f
#define MAX_WINDING 32

extern	BspPlane	mapPlanes[MAX_MAP_PLANES];
extern	int			numMapPlanes;

extern int		numMapVerts;
extern BspVertex	mapVerts[MAX_MAP_VERTS];

extern int		numMapFaceVerts;
extern int		mapFaceVerts[MAX_MAP_FACE_VERTS];

enum {	POLYGON_STRADDLING, 
		POLYGON_IN_FRONT, 
		POLYGON_BEHIND, 
		POLYGON_COPLANAR	};

void BspBoundBoxf::AddPoint(Vec3f p) {
	if(p.x < min.x) {
		min.x = p.x;
	}
	if(p.y < min.y) {
		min.y = p.y;
	}
	if(p.z < min.z) {
		min.z = p.z;
	}
	if(p.x > max.x) {
		max.x = p.x;
	}
	if(p.y > max.y) {
		max.y = p.y;
	}
	if(p.z > max.z) {
		max.z = p.z;
	}
}

BspVertex::BspVertex(float x, float y, float z) {
	point = Vec3f(x, y, z);
}

// Calculate a bounding box from a set of polygons
BspBoundBoxf CalcBounds(std::vector<BspFace *> &polygons) {
	BspBoundBoxf bounds = BspBoundBoxf(mapVerts[((*polygons[0]).vertIndices[0])].point);

	for(BspFace *polygon : polygons) {
		for(int vertIndex : polygon->vertIndices) {
			bounds.AddPoint(mapVerts[vertIndex].point);
		}
	}

	return bounds;
}

// Leaf node
BspNode::BspNode(std::vector<BspFace *> &polygons) {
	isLeaf = true;

	solid = polygons.size() == 0;
	this->faces = polygons;
	if(solid) {
		return;
	}

	bounds = CalcBounds(polygons);
}

// Internal node
BspNode::BspNode(BspNode *front, BspNode *back, int planeNum, std::vector<BspFace *> &polygons) {
	isLeaf = false;

	this->front = front;
	this->back = back;
	this->planeNum = planeNum;

	bounds = CalcBounds(polygons);

	this->faces = polygons;
}

// Find if a polygon is in front, behind, 
// coplanar, or straddling a plane
int ClassifyPolygonToPlane(BspFace *polygon, BspPlane plane) {
	int	numInFront, numBehind;
	numInFront = numBehind = 0;
	int numVerts = polygon->vertIndices.size();
	for(int i = 0; i < numVerts; i++) {
		Vec3f p = mapVerts[polygon->vertIndices[i]].point;

		float dist = plane.normal.Dot(p) - plane.dist;
		if(dist > PLANE_EPSILON) {
			numInFront++;
		}
		else if(dist < -PLANE_EPSILON) {
			numBehind++;
		}
	}

	if(numInFront != 0 && numBehind != 0) {
		return POLYGON_STRADDLING;
	}
	if(numInFront != 0) {
		return POLYGON_IN_FRONT;
	}
	if(numBehind != 0) {
		return POLYGON_BEHIND;
	}
	return POLYGON_COPLANAR;
}

// Pick the best splitting plane, heuristically
int PickSplittingPlane(std::vector<BspFace *> &polygons) {
	int bestPlaneNum = -1;
	float bestScore = FLOAT_MAX;

	for(int i = 0; i < polygons.size(); i++) {

		if(polygons[i]->tested) {	// Already tested
			continue;
		}

		int numInFront, numBehind, numStraddling = 0;
		int planeNum = polygons[i]->planeNum;
		BspPlane *plane = &mapPlanes[planeNum];
		for(int j = 0; j < polygons.size(); j++) {
			if(i == j) {	// Ignore testing against self
				continue;
			}
			if(polygons[j]->tested) {	// These are left unsplit
				continue;
			}
			switch(ClassifyPolygonToPlane(polygons[j], *plane)) {
			case POLYGON_COPLANAR:
				numInFront++;
				break;
			case POLYGON_IN_FRONT:
				numInFront++;
				break;
			case POLYGON_BEHIND:
				numBehind++;
				break;
			case POLYGON_STRADDLING:
				numStraddling++;
				break;
			}
		}

		float score = SPLIT_BALANCE * numStraddling + (1.0f - SPLIT_BALANCE) * fabs(numInFront - numBehind);
		if(score < bestScore) {
			bestScore = score;
			bestPlaneNum = planeNum;
		}
	}

	return bestPlaneNum;
}

Vec3f SegmentPlaneIntersection(Vec3f p1, Vec3f p2, BspPlane plane) {
	float d = (plane.normal.Dot(p1) - plane.dist) / plane.normal.Dot(p2 - p1);
	return p1 + d * (p2 - p1);
}

// Split the polygon and create two new polygons
void SplitPolygon(BspFace &polygon, BspPlane plane, BspFace **frontPoly, BspFace **backPoly) {
	int numFront, numBack;
	numFront = numBack = 0;

	int numVerts = polygon.vertIndices.size();
	int frontVerts[numVerts + 2], backVerts[numVerts + 2];
	int prev = polygon.vertIndices[numVerts - 1];
	int prevDot = plane.normal.Dot(mapVerts[prev].point) - plane.dist;

	

	for(int n = 0; n < numVerts; n++) {
		int curr = polygon.vertIndices[n];
		int currDot = plane.normal.Dot(mapVerts[curr].point) - plane.dist;
		if(currDot > PLANE_EPSILON) {
			if(prevDot < -PLANE_EPSILON) {
				// Current edge intersects plane,
				// So output the intersection point
				Vec3f intersection = SegmentPlaneIntersection(mapVerts[curr].point, mapVerts[prev].point, plane);
				mapVerts[numMapVerts] = intersection;
				frontVerts[numFront++] = backVerts[numBack++] = numMapVerts;
				numMapVerts++;
			}

			// Output b to front side in all three cases
			frontVerts[numFront++] = curr;
		}
		else if(currDot < -PLANE_EPSILON) {
			if(prevDot > PLANE_EPSILON) {
				// Also an intersection
				Vec3f intersection = SegmentPlaneIntersection(mapVerts[curr].point, mapVerts[prev].point, plane);
				mapVerts[numMapVerts] = intersection;
				frontVerts[numFront++] = backVerts[numBack++] = numMapVerts;
				numMapVerts++;
			}
			else if(prevDot > -PLANE_EPSILON) {	// Trailing vertex of edge is "on" plane
				backVerts[numBack++] = prev;
			}

			// Output b to back side in all three cases
			backVerts[numBack++] = curr;
		} else {
			// Leading vertex of edge is on the plane,
			// So output b to the front sde
			frontVerts[numFront++] = curr;
			if(prevDot < -PLANE_EPSILON) {
				backVerts[numBack++] = prev;
			}
		}

		// Set the trailing edge
		prev = curr; 
		prevDot = currDot;
	}

	*frontPoly = new BspFace(numFront, frontVerts, &polygon);
	*backPoly = new BspFace(numBack, backVerts, &polygon);
}

BspNode *BuildBspTree(std::vector<BspFace *> &polygons, int depth) {
	if(polygons.empty()) {
		return new BspNode(polygons);
	}

	int splitPlane = PickSplittingPlane(polygons);

	if(splitPlane == -1) {
		return new BspNode(polygons);
	}

	std::vector<BspFace *> nodeFaces, frontList, backList;

	int numPolys = polygons.size();
	for(int i = 0; i < numPolys; i++) {
		BspFace *polygon = polygons[i], *frontPart, *backPart;

		if(polygon->tested) {	// These are left unsplit
				continue;
		}

		switch(ClassifyPolygonToPlane(polygon, mapPlanes[splitPlane])) {
		case POLYGON_COPLANAR:
			polygon->tested = true; // Mark as tested so it won't recurse indefinitely
			nodeFaces.push_back(polygon);
		case POLYGON_IN_FRONT:
			frontList.push_back(polygon);
			break;
		case POLYGON_BEHIND:
			backList.push_back(polygon);
			break;
		case POLYGON_STRADDLING:
			SplitPolygon(*polygon, mapPlanes[splitPlane], &frontPart, &backPart);
			frontList.push_back(frontPart);
			backList.push_back(backPart);
			break;
		}
	}

	// Recurse on two children
	BspNode *frontTree = BuildBspTree(frontList, depth + 1);
	BspNode *backTree = BuildBspTree(backList, depth + 1);

	BspNode *node = new BspNode(frontTree, backTree, splitPlane, nodeFaces);
	node->bounds = CalcBounds(polygons);
	return node;
}

void BspModel::SetModel(Vec3f origin, Quaternion orientation) {
	this->origin = origin;
	this->orientation = orientation;
}

void BspModel::CreateTreeFromLazyMesh(LazyMesh *mesh) {
	std::cout << "--- Creating tree for BSP model ---" << std::endl;
	std::cout << mesh->faces.size() << " Initial number of faces" << std::endl;
	std::cout << mesh->vertexList.size() << " Initial number of verts" << std::endl;

	std::cout << "Applying offsets to map face verts..." << std::endl;
	for(BspFace *face : mesh->faces) {
		for(int vertIndex : face->vertIndices) {
			vertIndex += numMapVerts;
		}
	}

	std::cout << "Outputting verts to buffer..." << std::endl;
	for(BspVertex *vertex : mesh->vertexList) {
		mapVerts[numMapVerts] = *vertex;
		numMapVerts++;
	}

	solid = mesh->solid;
	root = BuildBspTree(mesh->faces, 0);

	std::cout << "Sucessful BuildBsp run..." << std::endl;
	std::cout << "Printing Tree..." << std::endl;

	PrintTree(root, 0);
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
}