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

	bool EqualTo(Vec3f normal, float dist);

	Vec3f	normal;
	float	dist;
	BspPlane *hashChain;
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
	BspFace(int numVerts, int vertIndices[], BspFace *face);
	BspFace(int numVerts, int vertIndices[], int planeNum);

	BspVertex GetVertex(int vertIndex);

	std::vector<int>	vertIndices;
	int			planeNum;
	Material	*material;

	int			outputNumber;
};

struct BspNode {
	BspNode() {}
	
	BspNode(std::vector<BspFace *> &polygons);
	BspNode(BspNode *front, BspNode *back, int planeNum, std::vector<BspFace *> &polygons);

	// Both leafs and internal nodes
	bool	isLeaf;
	int		depth;
	BspNode		*parent;
	BspBoundBoxf	bounds;
	std::vector<BspFace*>	faces;	// Used differently among leafs and internal nodes
	// Internal nodes only
	BspNode		*front, *back;
	int		planeNum;
	// Leafs only
	bool	solid;
};

struct LazyMesh {
	bool	solid;
	std::vector<BspFace*>	faces;
	std::vector<BspVertex*> vertexList;
};

struct BspModel {
	BspModel() {}

	void SetModel(Vec3f origin, Quaternion orientation);
	void CreateTreeFromLazyMesh(LazyMesh *mesh);

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

void PrintTree(BspNode *node, int depth);


int PlaneNumFromTriangle(Vec3f p0, Vec3f p1, Vec3f p2);
int FindPlane(Vec3f normal, float dist);

#endif