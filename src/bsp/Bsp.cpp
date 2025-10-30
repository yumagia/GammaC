
#include "Bsp.h"
#include "Math.h"
#include <vector>

#define PLANE_EPSILON 0.01
#define FLOAT_MAX 999999999
#define SPLIT_BALANCE 0.8f
#define MAX_WINDING 32

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
		max.x = p.y;
	}
	if(p.z > max.z) {
		max.z = p.z;
	}
}

// Calculate a bounding box from a set of polygons
BspBoundBoxf CalcBounds(std::vector<BspFace *> &polygons) {
	BspBoundBoxf bounds = BspBoundBoxf(((*polygons[0]).vertices[0])->point);

	for(BspFace *polygon : polygons) {
		for(BspVertex *vertex : polygon->vertices) {
			bounds.AddPoint(vertex->point);
		}
	}

	return bounds;
}

BspFace::BspFace(int numVerts, Vec3f verts[], BspPlane *plane) {
	vertices.reserve(numVerts);
	for(int i = 0; i < numVerts; i++) {
		vertices.push_back(new BspVertex(verts[i]));
	}

	this->plane = plane;
}

// Leaf node
BspNode::BspNode(std::vector<BspFace *> &polygons) {
	isLeaf = true;

	this->faces = polygons;
	minBounds = CalcBounds(polygons);
}

// Internal node
BspNode::BspNode(BspNode *front, BspNode *back, BspPlane *plane) {
	isLeaf = false;

	this->front = front;
	this->back = back;
	this->plane = plane;
}

// Find if a polygon is in front, behind, 
// coplanar, or straddling a plane
int ClassifyPolygonToPlane(BspFace *polygon, BspPlane plane) {
	int	numInFront, numBehind = 0;
	int numVerts = polygon->vertices.size();
	for(int i = 0; i < numVerts; i++) {
		Vec3f p = polygon->vertices[i]->point;

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
BspPlane PickSplittingPlane(std::vector<BspFace *> &polygons) {
	BspPlane bestPlane;
	float bestScore = FLOAT_MAX;

	for(int i = 0; i < polygons.size(); i++) {
		int numInFront, numBehind, numStraddling = 0;
		BspPlane plane = *polygons[i]->plane;
		for(int j = 0; j < polygons.size(); j++) {
			if(i == j) {	// Ignore testing against self
				continue;
			}
			switch(ClassifyPolygonToPlane(polygons[j], plane)) {
			case POLYGON_COPLANAR:
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
			bestPlane = plane;
		}
	}

	return bestPlane;
}

Vec3f SegmentPlaneIntersection(Vec3f p1, Vec3f p2, BspPlane plane) {
	float d = plane.normal.Dot(p2 - p1) - plane.dist;
	return p1 + d * (p2 - p1);
}

// Split the polygon and create two new polygons
void SplitPolygon(BspFace &polygon, BspPlane plane, BspFace **frontPoly, BspFace **backPoly) {
	int numFront, numBack = 0;

	int numVerts = polygon.vertices.size();
	Vec3f frontVerts[numVerts + 2], backVerts[numVerts + 2];
	Vec3f prev = polygon.vertices[numVerts - 1]->point;
	int prevDot = plane.normal.Dot(prev) - plane.dist;

	for(int n = 0; n < numVerts; n++) {
		Vec3f curr = polygon.vertices[n]->point;
		int currDot = plane.normal.Dot(curr) - plane.dist;
		if(currDot > PLANE_EPSILON) {
			if(prevDot < -PLANE_EPSILON) {
				// Current edge intersects plane,
				// So output the intersection point
				Vec3f intersection = SegmentPlaneIntersection(curr, prev, plane);
				frontVerts[numFront++] = backVerts[numBack++] = intersection;
			}

			// Output b to front side in all three cases
			frontVerts[numFront++] = curr;
		}
		else if(currDot < -PLANE_EPSILON) {
			if(prevDot > PLANE_EPSILON) {
				// Also an intersection
				Vec3f intersection = SegmentPlaneIntersection(curr, prev, plane);
				frontVerts[numFront++] = backVerts[numBack++] = intersection;
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

	*frontPoly = new BspFace(numFront, frontVerts, polygon.plane);
	*backPoly = new BspFace(numBack, backVerts, polygon.plane);
}

BspNode *BuildBspTree(std::vector<BspFace *> &polygons, int depth) {
	if(polygons.empty()) {
		return NULL;
	}

	int numPolys = polygons.size();

	if(numPolys <= 0) {
		return new BspNode(polygons);
	}

	BspPlane splitPlane = PickSplittingPlane(polygons);

	std::vector<BspFace *> frontList, backList;

	for(int i = 0; i < numPolys; i++) {
		BspFace *polygon = polygons[i], *frontPart, *backPart;
		switch(ClassifyPolygonToPlane(polygon, splitPlane)) {
		case POLYGON_COPLANAR:
		case POLYGON_IN_FRONT:
			frontList.push_back(polygon);
			break;
		case POLYGON_BEHIND:
			backList.push_back(polygon);
			break;
		case POLYGON_STRADDLING:
			SplitPolygon(*polygon, splitPlane, &frontPart, &backPart);
			frontList.push_back(frontPart);
			backList.push_back(backPart);
			break;
		}
	}

	// Recurse on two children
	BspNode *frontTree = BuildBspTree(frontList, depth + 1);
	BspNode *backTree = BuildBspTree(backList, depth + 1);
	return new BspNode(frontTree, backTree, &splitPlane);
}