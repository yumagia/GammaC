#ifndef BSP_LIB_INCLUDED
#define BSP_LIB_INCLUDED

#include "Math.h"
#include <vector>

/**=============================================
 * BSP OBJECTS
 * 
 * =============================================
*/

struct BspBoundBoxf {
	BspBoundBoxf() {}
	BspBoundBoxf(Vec3f min, Vec3f max) : min(min), max(max) {}
	BspBoundBoxf(Vec3f p) : min(p), max(p) {}

	void AddPoint(Vec3f p);
	
	Vec3f	min, max;
};

struct BspPlane {
	Vec3f	normal;
	float	dist;
};

struct BspVertex {
	BspVertex() {}
	BspVertex(Vec3f point) : point(point) {}

	Vec3f	point;
};

struct BspFace {
	BspFace() {}

	// Create fropm new winding, inherit plane
	BspFace(int numVerts, Vec3f verts[], BspPlane *plane);

	std::vector<BspVertex*>	vertices;
	BspPlane	*plane;
};

struct BspNode {
	BspNode() {}
	
	BspNode(std::vector<BspFace *> &polygons);
	BspNode(BspNode *front, BspNode *back, BspPlane *plane);

	bool	isLeaf;
	int		depth;
	// Internal nodes only
	BspNode		*front, *back;
	BspPlane	*plane;
	// Leafs only
	std::vector<BspFace*>	faces;
	BspBoundBoxf	minBounds;
};

struct BspModel {
	Vec3f	origin;

	BspNode root;
	
	BspBoundBoxf	minBounds;
};

#endif