#ifndef BSP_INCLUDED
#define BSP_INCLUDED

#include "Math.h"

#include <vector>

/**=============================================
 * BSP AND LAZY MESH OBJECTS
 * 
 * =============================================
*/

struct Material {
	Color	baseColor;
};

struct BoundBoxf {
	BoundBoxf() {}
	BoundBoxf(Vec3f min, Vec3f max) : min(min), max(max) {}
	BoundBoxf(Vec3f p) : min(p), max(p) {}

	void AddPoint(Vec3f p);
	
	Vec3f	min, max;
};

struct Plane {
	Plane() {}
	Plane(Vec3f normal, float dist) : normal(normal), dist(dist) {}

	Vec3f	normal;
	float	dist;
};

struct Vertex {
	Vertex() {}
	Vertex(Vec3f point) : point(point) {}

	// Create vertex from three floats
	Vertex(float x, float y, float z);

	Vec3f	point;
};

struct Face {
	Face() {}

	// Check if already tested for splitting
	bool		tested;

	// Create from new winding, inherit plane
	Face(int numVerts, Vec3f verts[], Face *face);
	Face(int numVerts, Vec3f verts[], Plane *plane);

	std::vector<Vertex*>	vertices;
	Plane	*plane;
	Material	*material;
};

struct Node {
	Node() {}
	
	Node(std::vector<Face *> &polygons);
	Node(Node *front, Node *back, Plane *plane);

	// Both leafs and internal nodes
	bool	isLeaf;
	int		depth;
	Node		*parent;
	BoundBoxf	minBounds;
	// Internal nodes only
	Node		*front, *back;
	Plane	*plane;
	// Leafs only
	bool	solid;
	std::vector<Face*>	faces;
};

struct LazyMesh {
	bool	solid;
	std::vector<Face*>	faces;
	std::vector<Vertex*> vertexList;
};

struct Model {
	Model() {}

	void SetModel(Vec3f origin, Quaternion orientation);
	void CreateTreeFromLazyMesh(LazyMesh mesh);

	bool solid;

	Vec3f	origin = Vec3f();
	Quaternion orientation = Quaternion();

	Node *root;
	
	BoundBoxf	minBounds;
};

/**=============================================
 * FUNCS
 * 
 * =============================================
*/

Plane *PlaneFromTriangle(Vec3f p0, Vec3f p1, Vec3f p2);

#endif