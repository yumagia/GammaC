#include "../Bsp.hpp"
#include "GammaFile.hpp"
#include "Math.hpp"

#include <iostream>
#include <vector>

int		numMapVerts;
BspVertex	mapVerts[MAX_MAP_VERTS];

int		numMapFaceVerts;
int		mapFaceVerts[MAX_MAP_FACE_VERTS];

BspFace::BspFace(std::vector<int> vertIndices, std::shared_ptr<BspFace> face) {
	if(vertIndices.size() < 3) {
		std::cerr << "WARNING: Bad vert count (" << vertIndices.size() << ") for face" << std::endl;
	}
	
	this->vertIndices = vertIndices;

	this->tested = face->tested;
	this->planeNum = face->planeNum;
	this->materialNum = face->materialNum;
}

BspFace::BspFace(std::vector<int> vertIndices, int planeNum, int materialNum) {
	if(vertIndices.size() < 3) {
		std::cerr << "WARNING: Bad vert count (" << vertIndices.size() << ") for face" << std::endl;
	}
	
	this->vertIndices = vertIndices;

	this->tested = false;
	this->planeNum = planeNum;
	this->materialNum = materialNum;
}