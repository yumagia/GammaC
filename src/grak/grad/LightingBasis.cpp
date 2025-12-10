#include "LightingBasis.hpp"

#include "Math.hpp"

LightingBasis::LightingBasis() {
	hBasis[0] = 0;
	hBasis[1] = 0;
	hBasis[2] = 0;
	hBasis[3] = 0;
	hBasis[4] = 0;
	hBasis[5] = 0;
}

LightingBasis::LightingBasis(const Vec3f &vector) {
	hBasis[0] = 0.3989422804f;
	hBasis[1] = -0.69098829894f * vector.x;
	hBasis[2] = 0.69098829894f * ((2 * vector.y) + 1);
	hBasis[3] = -0.69098829894f * vector.z;
	hBasis[4] = 1.54509680809 * vector.z * vector.x;
	hBasis[5] = 0.77254840404 * ((vector.y * vector.y) - (vector.z * vector.z));
}

LightingBasis::LightingBasis(const LightingBasis &otherBasis) {
	hBasis[0] = otherBasis.hBasis[0];
	hBasis[1] = otherBasis.hBasis[1];
	hBasis[2] = otherBasis.hBasis[2];
	hBasis[3] = otherBasis.hBasis[3];
	hBasis[4] = otherBasis.hBasis[4];
	hBasis[5] = otherBasis.hBasis[5];
}

float LightingBasis::Dot(const LightingBasis &otherBasis) {
	float dot = hBasis[0] * otherBasis.hBasis[0] + 
				hBasis[1] * otherBasis.hBasis[1] + 
				hBasis[2] * otherBasis.hBasis[2] + 
				hBasis[3] * otherBasis.hBasis[3] + 
				hBasis[4] * otherBasis.hBasis[4] + 
				hBasis[5] * otherBasis.hBasis[5];

	return dot;
}