#ifndef RADIOSITY_BAKER_INCLUDED
#define RADIOSITY_BAKER_INCLUDED

#include "GammaFile.hpp"

class RadiosityBaker {
public:
	RadiosityBaker();

	void BakeRad(BspFile &bspFile);
	
private:
	void InitLightMaps(BspFile &bspFile);

	void PatchesForFace(FileFace &face);

	void InitialLightingPass(BspFile &bspFile);

private:
	int numLumel = 0;
};

#endif