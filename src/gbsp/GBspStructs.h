#ifndef GBSP_STRUCTS_INCLUDED
#define GBSP_STRUCTS_INCLUDED

#include "Math.h"

struct BspLump {
	uint	size, offset;
};

struct BspHeader {
	BspLump lumps[7];
};

struct BspVertex {
	float x, y, z;
};

struct BspPlane {
	Vec3f	normal;
	float	dist;
};

struct BspBoundBoxf {
	Vec3f	min, max;
};

struct BspFace {
	uint	firstVert, numVerts;
	uint	planeNum;
	uint	materialNum;
};

struct BspNode {
	uint	parentIdx;
	uint	frontidx;
	uint	planeIdx;

	uint	aabbIdx;
};

struct BspLeaf {
	uint	firstFace, numFaces;

	uint	aabbIdx;
};

struct BspModel {
	uint	rootIdx;
};

#endif