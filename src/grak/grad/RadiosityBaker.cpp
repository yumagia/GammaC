#include "RadiosityBaker.hpp"

#include "LightingBasis.hpp"

void RadiosityBaker::InitLightMap(BspFile &bspFile) {
	for(FileFace face: bspFile.fileFaces) {
		
	}
}

void RadiosityBaker::InitialLightingPass(BspFile &bspFile) {

}

void RadiosityBaker::BakeRad(BspFile &bspFile) {
	InitLightMap(bspFile);
	InitialLightingPass(bspFile);
}