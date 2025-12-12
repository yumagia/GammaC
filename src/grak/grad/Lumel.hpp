#ifndef LUMEL_INCLUDED
#define LUMEL INCLUDED

#include "Math.hpp"
#include "LightingBasis.hpp"

struct Lumel {
	Vec3f	position, normal;
	
	LightingBasis	lightingBasis;

	int				faceIndex;
};

#endif