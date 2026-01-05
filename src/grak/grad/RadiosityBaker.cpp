#include "RadiosityBaker.hpp"

#include "LightingBasis.hpp"

RadiosityBaker::RadiosityBaker() {
	numLumel = 0;
}

void RadiosityBaker::PatchesForFace(FileFace &face) {
	face.lightMapOffset = numLumel;

	bspFile;
	face.planeNum;
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