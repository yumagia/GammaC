#ifndef BSP_INCLUDED
#define BSP_INCLUDED

#include "Math.hpp"

#include <vector>
#include <memory>

/**=============================================
 * BSP AND LAZY MESH OBJECTS
 * 
 * =============================================
*/

struct BspBoundBoxf;
struct BspPlane;
struct BspVertex;
struct BspFace;
struct BspNode;
class BspPortal;

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

	int		type;
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
	BspFace(std::vector<int> vertIndices, std::shared_ptr<BspFace> face);
	BspFace(std::vector<int> vertIndices, int planeNum, int materialNum);

	std::vector<int>	vertIndices;
	int			planeNum;
	int			materialNum;

	int			outputNumber;
};

struct BspNode {
	BspNode() {}
	
	BspNode(std::vector<std::shared_ptr<BspFace>> polygons);
	BspNode(BspNode *front, BspNode *back, int planeNum, std::vector<std::shared_ptr<BspFace>> polygons);

	// Both leafs and internal nodes
	bool	isLeaf;
	int		depth;
	BspNode		*parent;
	BspBoundBoxf	bounds;
	std::vector<std::shared_ptr<BspFace>>	faces;	// Used differently among leafs and internal nodes
	// Internal nodes only
	BspNode		*front, *back;
	int		planeNum;
	// Leafs only
	bool	solid;
	std::shared_ptr<BspPortal> portals = NULL;
};

class BspPortal {
	public:
		BspPortal();

		void CreateWindingFromNode(BspNode *node);		// Generates a "superportal" winding
		int Split(BspPlane *plane, BspPortal *front, BspPortal *back);
		
		void AddToNodes(BspNode *front, BspNode *back);
		int GetNextNodeSide(BspNode *node);
		int RemoveFromNode(BspNode *node);
		std::shared_ptr<BspPortal> GetNext(int side);
		BspNode *GetNextNode(int side);
		
		bool WindingValid();
	private:

		void Chop(BspPlane *plane);
	private:
		BspPlane	plane;
		
		BspNode		*nodes[2];
		std::shared_ptr<BspPortal> next[2];

		std::vector<Vec3f> winding;

		BspFace		*face;
};

struct LazyMesh {
	LazyMesh() {}
	~LazyMesh();

	bool	solid;
	std::vector<std::shared_ptr<BspFace>>	faces;
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

Vec3f SegmentPlaneIntersection(Vec3f p1, Vec3f p2, BspPlane plane);

void PrintTree(BspNode *node, int depth);
void FreeTree(BspNode *node);

int PlaneNumFromTriangle(Vec3f p0, Vec3f p1, Vec3f p2);
int FindPlane(Vec3f normal, float dist);

int TextureFromMaterial(int planeNum, int materialNum, Vec3f origin);

#endif