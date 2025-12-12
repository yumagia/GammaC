#include "RadiosityBaker.hpp"

#include "LightingBasis.hpp"

RadiosityBaker::RadiosityBaker() {
	numLumel = 0;
}

void RadiosityBaker::PatchesForFace(FileFace &face) {
	face.lightMapOffset = numLumel;

}

void RadiosityBaker::InitLightMaps(BspFile &bspFile) {
	for(FileFace face: bspFile.fileFaces) {
		PatchesForFace(face);
	}
}

void RadiosityBaker::InitialLightingPass(BspFile &bspFile) {

}

void RadiosityBaker::BakeRad(BspFile &bspFile) {
	InitLightMaps(bspFile);
	InitialLightingPass(bspFile);
}