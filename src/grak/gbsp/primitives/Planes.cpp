#include "Bsp.hpp"
#include "GammaFile.hpp"
#include "Math.hpp"

#include <iostream>
#include <vector>

int numMapPlanes;
BspPlane mapPlanes[MAX_MAP_PLANES];

#define PLANE_HASHES 1024
BspPlane	*hashPlanes[PLANE_HASHES];

#define AXIAL_EPSILON		0.999

#define NORMAL_EPSILON		0.000001
#define DISTANCE_EPSILON	0.001

int GetHashFromPlane(Vec3f normal, float dist) {
	int hash = (int)(abs(normal.x + 1) * 64)
		+ (int)(abs(normal.y + 1) * 64)
		+ (int)(abs(normal.z + 1) * 64)
		+ (int)abs(dist / 16);

	hash &= (PLANE_HASHES - 1);

	return hash;
}

int FindPlane(Vec3f normal, float dist) {
	BspPlane	*plane;
	int hash = GetHashFromPlane(normal, dist);

	for(plane = hashPlanes[hash]; plane; plane = plane->hashChain) {
		if(plane->EqualTo(normal, dist)) {
			return plane - mapPlanes;
		}
	}

	// Plane not found, so create it
	plane = &mapPlanes[numMapPlanes];
	plane->normal = normal;
	plane->dist = dist;

	if(normal.x > normal.y && normal.x > normal.z) {
		plane->type = 0;
		if(normal.x < AXIAL_EPSILON) {
			plane->type += 3;
		}
	}
	else if(normal.y > normal.z) {
		plane->type = 1;
		if(normal.y < AXIAL_EPSILON) {
			plane->type += 3;
		}
	}
	else {
		plane->type = 2;
		if(normal.z < AXIAL_EPSILON) {
			plane->type += 3;
		}
	}

	numMapPlanes++;

	plane->hashChain = hashPlanes[hash];
	hashPlanes[hash] = plane;

	return numMapPlanes - 1;
}


bool BspPlane::EqualTo(Vec3f normal, float dist) {
	if(	fabs(this->dist - dist) > DISTANCE_EPSILON
	|| fabs(this->normal.x - normal.x) > NORMAL_EPSILON
	|| fabs(this->normal.y - normal.y) > NORMAL_EPSILON
	|| fabs(this->normal.z - normal.z) > NORMAL_EPSILON ) {
		return false;
	}
	
	return true;
}

int PlaneNumFromTriangle(Vec3f p0, Vec3f p1, Vec3f p2) {
	Vec3f a = p1 - p0;
	Vec3f b = p2 - p0;
	Vec3f normal = a.Cross(b);
	normal.Normalize();
	float d = normal.Dot(p0);

	return FindPlane(normal, d);
}