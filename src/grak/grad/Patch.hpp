#define MAX_MAP_PATCHES 262144

#ifndef PATCH_INCLUDED
#define PATCH_INCLUDED

#include "Math.hpp"

struct Transfer {
	unsigned int	patch;
	unsigned int	transfer;
};

class Patch {
public:
	int				legal;
	int				faceIndex;

	Color			accumulatedLight;

	Color			diffuse, emissive;

	Color			sampledLight;
};

#endif