#ifndef RADIOSITY_BAKER_INCLUDED
#define RADIOSITY_BAKER_INCLUDED

#include "GammaFile.hpp"

#include "Math.hpp"


class RadiosityBaker {
public:
	RadiosityBaker();

	// Creates lightmaps and bakes the lighting for the given world model, returns numLumels
	int BakeRad(BspFile *bspFile);
	// Also returns numLumels
	int GetNumLumels();
	
private:
	void InitLightMaps();
	void InitialLightingPass();

	void PatchesForFace(FileFace *face);
	bool SampleLegal(Vec3f samplePosition, FileFace *face);
	void CollectLighting(FileLumel *lumel);
private:
	BspFile *bspFile = nullptr;

	int numLumels = 0;
};

#endif