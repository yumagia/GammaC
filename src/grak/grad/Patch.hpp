#define MAX_MAP_PATCHES 262144

#ifndef PATCH_INCLUDED
#define PATCH_INCLUDED

#include "GammaFile.hpp"
#include "Math.hpp"

struct Transfer {
	unsigned int	patch;
	float			transfer;
};

class Patch {
public:
	void	CalcTransfersForpatch(int numPatches, Patch *patchList, BspFile *bspFile);
	void	ShootLight(Color *collected);
	void	CollectLight(Color light);

	void	NudgePosition(BspFile *bspFile);

	void	FreeTransfers();

	int		legal;
	int		faceIndex;

	int			numTransfers;

	float		escapeRate;

	Transfer	*transfers = nullptr;
	Vec3f		position;

	Color	accumulatedLight;
	Color	diffuse, emissive;
	Color	sampledLight;
};

#endif