#ifndef LIGHT_TRACE_INCLUDED
#define LIGHT_TRACE_INCLUDED

#include "GammaFile.hpp"

#include "Math.hpp"


class Trace {
public: 
	Trace(BspFile *bspFile);
	bool TraceLine(Vec3f startPos, Vec3f endPos);

	Vec3f	hitPoint;
	float	hitFraction = 0;
private:
	bool TraceLine_r(int nodeIdx);

	Vec3f startPos, endPos;

	FileNode *fileNodes;
	FilePlane *filePlanes;
};


#endif