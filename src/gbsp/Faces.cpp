#include "Bsp.hpp"
#include "GammaFile.h"
#include "Math.hpp"

#include <vector>

int		numMapVerts;
BspVertex	mapVerts[MAX_MAP_VERTS];

int		numMapFaceVerts;
int		mapFaceVerts[MAX_MAP_FACE_VERTS];

BspFace::BspFace(int numVerts, int vertIndices[], BspFace *face) {
	this->vertIndices.reserve(numVerts);
	for(int i = 0; i < numVerts; i++) {
		this->vertIndices.push_back(vertIndices[i]);
	}

	this->tested = face->tested;
	this->planeNum = face->planeNum;
}

BspFace::BspFace(int numVerts, int vertIndices[], int planeNum) {
	this->vertIndices.reserve(numVerts);
	for(int i = 0; i < numVerts; i++) {
		this->vertIndices.push_back(vertIndices[i]);
	}

	this->planeNum = planeNum;
}