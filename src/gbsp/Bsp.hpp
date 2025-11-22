#ifndef BSP_INCLUDED
#define BSP_INCLUDED

#include "Math.hpp"

#include <vector>

/**=============================================
 * BSP AND LAZY MESH OBJECTS
 * 
 * =============================================
*/

struct Material {
	Color	baseColor;
};

struct BspBoundBoxf {
	BspBoundBoxf() {}
	BspBoundBoxf(Vec3f min, Vec3f max) : min(min), max(max) {}
	BspBoundBoxf(Vec3f p) : min(p), max(p) {}

	void AddPoint(Vec3f p);
	
	Vec3f	min, max;
};

struct BspPlane {
	BspPlane() {}
	BspPlane(Vec3f normal, float dist) : normal(normal), dist(dist) {}

	Vec3f	normal;
	float	dist;
};

struct BspVertex {
	BspVertex() {}
	BspVertex(Vec3f point) : point(point) {}

	// Create vertex from three floats
	BspVertex(float x, float y, float z);

	Vec3f	point;
};

struct BspFace {
	BspFace() {}

	// Check if already tested for splitting
	bool		tested;

	// Create from new winding, inherit plane
	BspFace(int numVerts, Vec3f verts[], BspFace *face);
	BspFace(int numVerts, Vec3f verts[], BspPlane *plane);

	std::vector<BspVertex*>	vertices;
	BspPlane	*plane;
	Material	*material;
};

struct BspNode {
	BspNode() {}
	
	BspNode(std::vector<BspFace *> &polygons);
	BspNode(BspNode *front, BspNode *back, BspPlane *plane, std::vector<BspFace *> &polygons);

	// Both leafs and internal nodes
	bool	isLeaf;
	int		depth;
	BspNode		*parent;
	BspBoundBoxf	bounds;
	// Internal nodes only
	BspNode		*front, *back;
	BspPlane	*plane;
	// Leafs only
	bool	solid;
	std::vector<BspFace*>	faces;
};

struct LazyMesh {
	bool	solid;
	std::vector<BspFace*>	faces;
	std::vector<BspVertex*> vertexList;
};

struct BspModel {
	BspModel() {}

	void SetModel(Vec3f origin, Quaternion orientation);
	void CreateTreeFromLazyMesh(LazyMesh mesh);

	bool solid;

	Vec3f	origin = Vec3f();
	Quaternion orientation = Quaternion();

	BspNode *root;
	
	BspBoundBoxf	bounds;
};

/**=============================================
 * FUNCS
 * 
 * =============================================
*/

BspPlane *PlaneFromTriangle(Vec3f p0, Vec3f p1, Vec3f p2);

#endif