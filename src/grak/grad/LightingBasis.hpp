#ifndef LIGHTING_BASIS_INCLUDED
#define LIGHTING_BASIS_INCLUDED

#include "Math.hpp"

#include <vector>

class LightingBasis {
	LightingBasis();
	LightingBasis(const Vec3f &vector);
	LightingBasis(const LightingBasis &otherBasis);

	float Dot(const LightingBasis &otherBasis);

	float hBasis[6];
};

#endif