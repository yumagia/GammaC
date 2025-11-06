
#include "GBspMap.h"

#define MAX_VERTICES	16384
#define MAX_PLANES		2048
#define MAX_FACES		4096
#define MAX_NODES		2048
#define MAX_LEAFS		1024
#define MAX_MODELS		128

GBspMap::GBspMap() {
	vertices = new BspVertex[MAX_VERTICES];
	planes = new BspPlane[MAX_PLANES];
	faces = new BspFace[MAX_FACES];
	nodes = new BspNode[MAX_NODES];
	leafs = new BspLeaf[MAX_LEAFS];
	models = new BspModel[MAX_MODELS];
}

GBspMap::~GBspMap() {
	delete[] vertices;
	delete[] planes;
	delete[] faces;
	delete[] nodes;
	delete[] leafs;
	delete[] models;
}