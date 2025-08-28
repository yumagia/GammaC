#include "cmdlib.h"
#include "veclib.h"
#include <cmath>

vec3_t vec3_origin = {0,0,0};


double	VectorLength(vec3_t v) {
	int				i;
	double			length;

	length = 0;
	for (i=0; i< 3; i++)
		length += v[i]*v[i];
	length = sqrt(length);

	return length;
}

bool VectorCompare(vec3_t v1, vec3_t v2) {
	int		i;

	for(i = 0; i < 3; i++) {
		if(fabs(v1[i] - v2[i]) > EQUAL_EPSILON) {
			return false;
		}
	}

	return true;
}

vec_t	VectorNormalize(vec3_t in, vec3_t out) {
	vec_t	length, ilength;

	length = sqrt(in[0]*in[0] + in[1]*in[1] + in[2]*in[2]);
	if (length == 0) {
		VectorClear(out);
		return 0;
	}

	ilength = 1.0/length;
	out[0] = in[0]*ilength;
	out[1] = in[1]*ilength;
	out[2] = in[2]*ilength;

	return length;
}

void VectorMA(vec3_t va, double scale, vec3_t vb, vec3_t vc) {
	vc[0] = va[0] + scale*vb[0];
	vc[1] = va[1] + scale*vb[1];
	vc[2] = va[2] + scale*vb[2];
}

void CrossProduct(vec3_t v1, vec3_t v2, vec3_t cross) {
	cross[0] = v1[1]*v2[2] - v1[2]*v2[1];
	cross[1] = v1[2]*v2[0] - v1[0]*v2[2];
	cross[2] = v1[0]*v2[1] - v1[1]*v2[0];
}

void VectorInverse(vec3_t v) {
	v[0] = -v[0];
	v[1] = -v[1];
	v[2] = -v[2];
}

void ClearBounds(vec3_t mins, vec3_t maxs) {
	mins[0] = mins[1] = mins[2] = 99999;
	maxs[0] = maxs[1] = maxs[2] = -99999;
}

void AddPointToBounds(vec3_t v, vec3_t mins, vec3_t maxs) {
	int		i;
	vec_t	val;

	for (i=0 ; i<3 ; i++) {
		val = v[i];
		if (val < mins[i]) {
			mins[i] = val;
		}
		if (val > maxs[i]) {
			maxs[i] = val;
		}
	}
}