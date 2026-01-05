#include "RadiosityBaker.hpp"

#include "LightingBasis.hpp"

RadiosityBaker::RadiosityBaker() {
	numLumel = 0;
}

void RadiosityBaker::PatchesForFace(FileFace &face) {
	face.lightMapOffset = numLumel;

	FilePlane *facePlane = &bspFile->filePlanes[face.planeNum];
	Vec3f normal(facePlane->normal[0], facePlane->normal[1], facePlane->normal[2]);
	
}

void RadiosityBaker::InitLightMaps() {
	for(FileFace face: bspFile->fileFaces) {
		PatchesForFace(face);
	}
}

void RadiosityBaker::InitialLightingPass() {

}

void RadiosityBaker::BakeRad(BspFile *bspFile) {
	this->bspFile = bspFile;

	InitLightMaps();
	InitialLightingPass();
}