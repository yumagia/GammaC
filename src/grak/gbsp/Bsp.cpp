#include "GammaFile.hpp"
#include "Bsp.hpp"
#include "Math.hpp"

#include <vector>
#include <iostream>
#include <memory>

#define PLANE_EPSILON 0.01
#define FLOAT_MAX 999999999
#define SPLIT_BALANCE 0.8f
#define MAX_TREE_DEPTH 200

extern	BspPlane	mapPlanes[MAX_MAP_PLANES];
extern	int			numMapPlanes;

extern int		numMapVerts;
extern BspVertex	mapVerts[MAX_MAP_VERTS];

extern int		numMapFaceVerts;
extern int		mapFaceVerts[MAX_MAP_FACE_VERTS];

int splitFaces;

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
BspBoundBoxf CalcBounds(std::vector<std::shared_ptr<BspFace>> polygons) {
	if(polygons.empty()) {
		return BspBoundBoxf(Vec3f());
	}

	BspBoundBoxf bounds = BspBoundBoxf(mapVerts[((*polygons[0]).vertIndices[0])].point);

	for(std::shared_ptr<BspFace> polygon : polygons) {
		for(int vertIndex : polygon->vertIndices) {
			bounds.AddPoint(mapVerts[vertIndex].point);
		}
	}

	return bounds;
}

// Leaf node
BspNode::BspNode(std::vector<std::shared_ptr<BspFace>> polygons) {
	isLeaf = true;

	solid = polygons.size() == 0;
	this->faces = polygons;
	if(solid) {
		return;
	}

	bounds = CalcBounds(polygons);
}

// Internal node
BspNode::BspNode(BspNode *front, BspNode *back, int planeNum, std::vector<std::shared_ptr<BspFace>> polygons) {
	isLeaf = false;

	this->front = front;
	this->back = back;
	this->planeNum = planeNum;

	this->faces = polygons;
}

// Find if a polygon is in front, behind, 
// coplanar, or straddling a plane
int ClassifyPolygonToPlane(std::shared_ptr<BspFace> polygon, BspPlane plane) {
	int	numInFront = 0;
	int numBehind = 0;
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
int PickSplittingPlane(std::vector<std::shared_ptr<BspFace>> polygons) {
	int bestPlaneNum = -1;
	std::shared_ptr<BspFace> bestPoly = NULL;
	float bestScore = FLOAT_MAX;

	for(int i = 0; i < polygons.size(); i++) {

		if(polygons[i]->tested) {	// Already tested
			continue;
		}

		int numInFront = 0;
		int numBehind = 0;
		int numStraddling = 0;

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
			bestPoly = polygons[i];
		}
	}

	if(bestPoly) {
		bestPlaneNum = bestPoly->planeNum;
	}

	return bestPlaneNum;
}

Vec3f SegmentPlaneIntersection(Vec3f p1, Vec3f p2, BspPlane plane) {
	float d = -(plane.normal.Dot(p1) - plane.dist) / plane.normal.Dot(p2 - p1);
	return p1 + d * (p2 - p1);
}

// Split the polygon and create two new polygons
void SplitPolygon(std::shared_ptr<BspFace> polygon, BspPlane plane, std::shared_ptr<BspFace> &frontPoly, std::shared_ptr<BspFace> &backPoly) {
	frontPoly = backPoly = NULL;

	int numVerts = polygon->vertIndices.size();
	std::vector<int> frontVerts, backVerts;
	int prev = polygon->vertIndices[numVerts - 1];
	int prevDot = plane.normal.Dot(mapVerts[prev].point) - plane.dist;

	for(int n = 0; n < numVerts; n++) {
		int curr = polygon->vertIndices[n];
		int currDot = plane.normal.Dot(mapVerts[curr].point) - plane.dist;
		if(currDot > PLANE_EPSILON) {
			if(prevDot < -PLANE_EPSILON) {
				// Current edge intersects plane,
				// So output the intersection point
				Vec3f intersection = SegmentPlaneIntersection(mapVerts[curr].point, mapVerts[prev].point, plane);
				mapVerts[numMapVerts] = intersection;
				frontVerts.push_back(numMapVerts);
				backVerts.push_back(numMapVerts);
				numMapVerts++;
			}

			// Output b to front side in all three cases
			frontVerts.push_back(curr);
		}
		else if(currDot < -PLANE_EPSILON) {
			if(prevDot > PLANE_EPSILON) {
				// Also an intersection
				Vec3f intersection = SegmentPlaneIntersection(mapVerts[curr].point, mapVerts[prev].point, plane);
				mapVerts[numMapVerts] = intersection;
				frontVerts.push_back(numMapVerts);
				backVerts.push_back(numMapVerts);
				numMapVerts++;
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

	splitFaces++;

	if(frontVerts.size() >= 3) {
		frontPoly = std::make_shared<BspFace>(frontVerts, polygon);
	}
	if(backVerts.size() >= 3) {
		backPoly = std::make_shared<BspFace>(backVerts, polygon);
	}
}

BspNode *BuildBspTree(std::vector<std::shared_ptr<BspFace>> &polygons, int depth) {
	if(depth > MAX_TREE_DEPTH) {
		return new BspNode(polygons);
	}

	if(polygons.empty()) {
		return new BspNode(polygons);
	}

	int splitPlane = PickSplittingPlane(polygons);

	if(splitPlane == -1) {
		return new BspNode(polygons);
	}

	std::vector<std::shared_ptr<BspFace>> nodeFaces, frontList, backList;

	int numPolys = polygons.size();
	for(int i = 0; i < numPolys; i++) {
		std::shared_ptr<BspFace> polygon = polygons[i], frontPart, backPart;

		switch(ClassifyPolygonToPlane(polygon, mapPlanes[splitPlane])) {
		case POLYGON_COPLANAR:
			nodeFaces.push_back(polygon);
			polygon->tested = true; // Mark as tested so it won't recurse indefinitely
			if(mapPlanes[splitPlane].normal.Dot(mapPlanes[polygon->planeNum].normal) < 0) {
				backList.push_back(polygon);
			}
			else {
				frontList.push_back(polygon);
			}
			break;
		case POLYGON_IN_FRONT:
			frontList.push_back(polygon);
			break;
		case POLYGON_BEHIND:
			backList.push_back(polygon);
			break;
		case POLYGON_STRADDLING:
			if(polygon->tested) {	// These are left unsplit
				frontList.push_back(polygon);
				backList.push_back(polygon);
			}
			else {
				SplitPolygon(polygon, mapPlanes[splitPlane], frontPart, backPart);
				if(frontPart) {
					frontList.push_back(frontPart);

				}
				if(backPart) {
					backList.push_back(backPart);
				}
			}
			
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
	std::cout << "--- BuildBSP ---" << std::endl;
	std::cout << "Creating tree for BSP model..." << std::endl;
	std::cout << "\t" << mesh->faces.size() << " Initial number of faces" << std::endl;
	std::cout << "\t" << mesh->vertexList.size() << " Initial number of verts" << std::endl;

	std::cout << "Applying offsets to map face verts..." << std::endl;
	for(std::shared_ptr<BspFace> face : mesh->faces) {
		for(int vertIndex : face->vertIndices) {
			vertIndex += numMapVerts;
		}
	}

	std::cout << "Outputting verts to buffer..." << std::endl;
	for(BspVertex *vertex : mesh->vertexList) {
		mapVerts[numMapVerts] = *vertex;
		numMapVerts++;
	}

	for(int i = 0; i < numMapPlanes; i++) {
		BspPlane *plane = &mapPlanes[i];
	}

	std::cout << "Building tree..." << std::endl;

	solid = mesh->solid;
	root = BuildBspTree(mesh->faces, 0);

	std::cout << "Sucessful BuildBSP run..." << std::endl;

	std::cout << "\t" << mesh->faces.size() + splitFaces << " Resulting number of faces" << std::endl;
	std::cout << "\t" << numMapVerts << " Resulting number of vertices" << std::endl;

	// std::cout << "Printing Tree..." << std::endl;

	// PrintTree(root, 0);
}