#ifndef LIGHT_TRACE_INCLUDED
#define LIGHT_TRACE_INCLUDED

#include "GammaFile.hpp"

#include "Math.hpp"


class Trace {
public: 
	Trace(BspFile *bspFile);

	// Simple, minimal line trace which finds the first plane of impact
	bool FastTraceLine(Vec3f startPos, Vec3f endPos);

	bool	startSolid = true;
	int		hitNodeIdx = -1;
	Vec3f	hitPoint;
	float	hitFraction = 0;
private:
	bool FastTraceLine_r(int parentIdx, int nodeIdx);

	Vec3f startPos, endPos;

	FileNode	*fileNodes;
	FileLeaf	*fileLeafs;
	FilePlane	*filePlanes;
};


#endif