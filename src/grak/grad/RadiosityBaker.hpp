#ifndef RADIOSITY_BAKER_INCLUDED
#define RADIOSITY_BAKER_INCLUDED
#include "GammaFile.hpp"

#include "Patch.hpp"
#include "LightingBasis.hpp"
#include "lighttrace/Trace.hpp"

#include "Math.hpp"


class RadiosityBaker {
public:
	RadiosityBaker();

	// Creates lightmaps and bakes the lighting for the given world model, returns numLumels
	int		BakeRad(BspFile *bspFile);
	// Also returns numLumels
	int		GetNumLumels();
	
private:
	void	InitLightMaps();
	void	InitialLightingPass();
	void	CreatePatchTransfers();

	void	PatchesForFace(FileFace *face);
	bool	SampleIsLegal(Vec3f samplePosition, FileFace *face);
	bool	SquareSampleIsLegal(Vec3f samplePosition, float extent, FileFace *face);
	void	CollectLightingForFace(FileFace *face);
	void	CollectLightingForPatch(Patch *patch, Vec3f samplePosition);
	float	SampleNormalDistribution();
	Vec3f	SampleUnitSphere();
	int		FindStruckFace(FileNode *node, Vec3f position);
	int		FindNodeLumel(FileNode *node, Vec3f position);
	int		FindFaceLumel(FileFace *face, Vec3f position);
private:
	BspFile *bspFile = nullptr;

	Patch *patchList = nullptr;

	int numLumels = 0;
};

#endif