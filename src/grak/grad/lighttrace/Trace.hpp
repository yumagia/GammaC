#ifndef LIGHT_TRACE_INCLUDED
#define LIGHT_TRACE_INCLUDED

#include "GammaFile.hpp"

#include "Math.hpp"


class Trace {
public: 
	Trace(BspFile *bspFile);

	bool PositionSolid(Vec3f position);
	bool LineStab(Vec3f startPos, Vec3f endPos);
	bool TraceLine(Vec3f startPos, Vec3f endPos);

	bool	startSolid = true;
	int		hitNodeIdx = -1;
	Vec3f	hitPoint;
	float	hitFraction = 0;
private:
	bool PositionSolid_r(int nodeIdx);
	bool LineStab_r(int nodeIdx);
	bool TraceLine_r(int nodeIdx, Vec3f startPos, Vec3f endPos);

	Vec3f startPos, endPos;

	FileNode	*fileNodes;
	FileLeaf	*fileLeafs;
	FilePlane	*filePlanes;
};


#endif