#define PLANENUM_LEAF		-1
#define MAX_EDGES			20
#define MAX_NODE_BRUSHES	8


#ifndef GBSP_INCLUDED
#define GBSP_INCLUDED

#include "BspFile.h"
#include "Math.h"
#include "Winding.h"
#include "BspFile.h"

struct Plane {
	Vec3f	normal;
	float	dist;
	int		type;
	Plane	*next;
};

struct Side {
	int		planeIdx;
	Winding	*winding;
	Side	*original;

	bool	visible;
	bool	tested;		// True if used, never reused if true
	bool	bevel;		// Never a splitting plane
};

struct BoundBox {
	Vec3f min, max;
};

struct MapBrush {
	int		entityIdx;
	int		brushIdx;

	int		contents;

	BoundBox	bounds;

	int		numSides;
	Side	*originalSides;
};

struct Face {
	Face	*next;
	Face	*merged;
	Face	*split[2];

	Portal	*portal;
	int		planeIdx;
	int		contents;
	int		outputNum;
	Winding	*winding;
	int		numPoints;
	Side	*original;

	int		vertexNums[MAX_EDGES];
};

struct BspBrush {
	int			id;
	BspBrush	*next;
	BoundBox	bounds;
	int			side;
	int			testSide;
	MapBrush	*original;
	int			numsides;
	Side		sides[6];
};

struct LeafFace {
	Face		*facePtr;
	LeafFace	*next;
};

struct Node {
	int			id;

	// Both leafs and internal nodes
	int			planeIdx;
	Node		*parent;
	BoundBox	bounds;
	BspBrush	*volume;
	// Non-leaf nodes only
	bool		detailSeperated;
	Side		*side;
	Node		*children[2];
	Face		*faces;
	// Leafs only
	BspBrush	*brushList;
	LeafFace	*leafFaceList;
	int			contents;
	int			occupied;
	Entity		*occupant;
	int			cluster;
	int			area;
	Portal		*portals;
};

struct Portal {
	int			id;
	Plane		plane;
	Node		*onNode;
	Node		*nodes[2];
	Portal		*next[2];
	Winding		*winding;
	bool		sideFound;
	Side		*side;
	Face		*face[2];
};

struct Tree {
	Node		*headNode;
	Node		*outsideNode;
	BoundBox	bounds;
};

class GBsp {
public: 
	GBsp() {}

	int Run(int argc, char **argv);
private:
	const char *fileName;

	MapLoader mapLoader;
};

#endif