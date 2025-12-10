#ifndef RADIOSITY_BAKER_INCLUDED
#define RADIOSITY_BAKER_INCLUDED

#include "GammaFile.hpp"

class RadiosityBaker {
public:
	RadiosityBaker() {}

	void BakeRad(BspFile &bspFile);
	
private:
	void InitLightMap(BspFile &bspFile);

	void InitialLightingPass(BspFile &bspFile);

private:
	BspFile *bspFile;
};

#endif