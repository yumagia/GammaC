#include "../Bsp.hpp"
#include "GammaFile.hpp"
#include "Math.hpp"

#include <iostream>
#include <vector>

extern	BspPlane	mapPlanes[MAX_MAP_PLANES];
extern	int			numMapPlanes;

int TextureFromMaterial(int planeNum, int materialNum, Vec3f origin) {
	Vec3f		uAxis, vAxis;
	float		uDist, vDist;

	Vec3f normal = mapPlanes[planeNum].normal;

	float max = normal.x;
	float majorAxis = 0;
	if(fabs(max) < fabs(normal.y)) {
		max = normal.y;
		majorAxis = 1;
	}
	if(fabs(max) < fabs(normal.z)) {
		max = normal.z;
		majorAxis = 2;
	}

	

	uDist = origin.Dot(uAxis);
	vDist = origin.Dot(vAxis);

	return 1;
}