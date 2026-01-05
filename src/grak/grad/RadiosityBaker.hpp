#ifndef RADIOSITY_BAKER_INCLUDED
#define RADIOSITY_BAKER_INCLUDED

#include "GammaFile.hpp"

class RadiosityBaker {
public:
	RadiosityBaker();

	void BakeRad(BspFile *bspFile);
	
private:
	void InitLightMaps();

	void PatchesForFace(FileFace &face);

	void InitialLightingPass();

private:
	BspFile *bspFile = nullptr;

	int numLumel = 0;
};

#endif