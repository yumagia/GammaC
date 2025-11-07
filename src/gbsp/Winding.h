#define MAX_POINTS_ON_WINDING	32
#define ON_EPSILON				0.1

#ifndef WINDING_INCLUDED
#define WINDING_INCLUDED

#include "Math.h"
#include "GBsp.h"

struct Winding {
	Vec3f	GetArea();
	Vec3f	GetCenter();

	void	ChopWindingInPlace(Plane plane, float epsilon);
	


	int			numPoints, maxPoints;
	Vec3f		*p;
	Winding		*next;
};

Winding		*AllocWinding(int numPoints);

#endif