#ifndef PORTAL_INCLUDED
#define PORTAL_INCLUDED

#include "GammaFile.hpp"
#include "Math.hpp"

#include <vector>

struct Plane {
	Vec3f normal;
	float dist;
};

class Portal {
	public:
		Portal(BspFile *bspFile);

		void CreateWindingFromNode(FileNode *node);

		int Split(Plane *plane, Portal &front, Portal &back);
		Vec3f SegmentPlaneIntersection(Vec3f p1, Vec3f p2, Plane *plane);
	private:
		BspFile *bspFile;

		Plane	plane;
		int		leaf;

		Vec3f	center;
		float	radius;

		std::vector<Vec3f> winding;
};

#endif