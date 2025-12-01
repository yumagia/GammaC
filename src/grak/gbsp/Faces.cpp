#include "Bsp.hpp"
#include "GammaFile.h"
#include "Math.hpp"

#include <iostream>
#include <vector>

int		numMapVerts;
BspVertex	mapVerts[MAX_MAP_VERTS];

int		numMapFaceVerts;
int		mapFaceVerts[MAX_MAP_FACE_VERTS];

BspFace::BspFace(int numVerts, int vertIndices[], BspFace *face) {
	if(numVerts < 3) {
		std::cerr << "WARNING: Bad vert count (" << numVerts << ") for face" << std::endl;
	}
	
	this->vertIndices.reserve(numVerts);
	for(int i = 0; i < numVerts; i++) {
		this->vertIndices.push_back(vertIndices[i]);
	}

	this->tested = face->tested;
	this->planeNum = face->planeNum;
}

BspFace::BspFace(int numVerts, int vertIndices[], int planeNum) {
	if(numVerts < 3) {
		std::cerr << "WARNING: Bad vert count (" << numVerts << ") for face" << std::endl;
	}
	
	this->vertIndices.reserve(numVerts);
	for(int i = 0; i < numVerts; i++) {
		this->vertIndices.push_back(vertIndices[i]);
	}

	this->tested = false;
	this->planeNum = planeNum;
}