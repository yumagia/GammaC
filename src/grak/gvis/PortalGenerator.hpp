#ifndef PORTAL_GENERATOR_INCLUDED
#define PORTAL_GENERATOR_INCLUDED

#include "GammaFile.hpp"

#include "Portal.hpp"
#include "Math.hpp"

#include <vector>

struct Leaf {
	int numPortals;
	std::vector<Portal> portals;
};

class PortalGenerator {
	public:
		void GeneratePortals(BspFile *bspFile);
		
	private:
		void GenerateWorldBackTraces();
		void GenerateWorldBackTraces_r(int nodeIdx, int parentIdx);
		void GeneratePortals_r(FileNode *node, FileNode *parent);

	private:
		BspFile *bspFile;
		std::vector<int> nodeParents;
};

#endif