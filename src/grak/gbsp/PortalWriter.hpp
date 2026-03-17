#ifndef PORTAL_GENERATOR_INCLUDED
#define PORTAL_GENERATOR_INCLUDED

#include "Bsp.hpp"

#include "Math.hpp"

#include <vector>

class PortalWriter {
	public:
		void WritePortals(BspNode *node);

	private:
		void GeneratePortals_r(BspNode *node);
		void FillLeafNumbers_r(BspNode *node, int num);
		void NumberLeafs_r(BspNode *node);
		void CreateNodePortals(BspNode *node);
		void SplitNodePortals(BspNode *node);
		void AddPortalToNodes(std::shared_ptr<BspPortal> portal, BspNode *front, BspNode *back);

	private:
		int		numVisClusters, numVisPortals;
};

#endif