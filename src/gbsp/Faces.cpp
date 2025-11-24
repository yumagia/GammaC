#include "Bsp.hpp"
#include "GammaFile.h"
#include "Math.hpp"

#include <vector>

int		numMapVerts;
BspVertex	mapVerts[MAX_MAP_VERTS];

int		numMapFaceVerts;
int		mapFaceVerts[MAX_MAP_FACE_VERTS];



BspFace::BspFace(int numVerts, Vec3f verts[], BspFace *face) {
	vertices.reserve(numVerts);
	for(int i = 0; i < numVerts; i++) {
		vertices.push_back(new BspVertex(verts[i]));
	}

	this->tested = face->tested;
	this->planeNum = face->planeNum;
}

BspFace::BspFace(int numVerts, Vec3f verts[], int planeNum) {
	vertices.reserve(numVerts);
	for(int i = 0; i < numVerts; i++) {
		vertices.push_back(new BspVertex(verts[i]));
	}

	this->planeNum = planeNum;
}