#ifndef RADIOSITY_BAKER_INCLUDED
#define RADIOSITY_BAKER_INCLUDED

#include "GammaFile.hpp"

#include "Math.hpp"


class RadiosityBaker {
public:
	RadiosityBaker();

	void BakeRad(BspFile *bspFile);
	
private:
	void InitLightMaps();
	void InitialLightingPass();

	void PatchesForFace(FileFace *face);

	bool SampleLegal(Vec3f samplePosition, FileFace *face);
private:
	BspFile *bspFile = nullptr;

	int numLumel = 0;
};

#endif