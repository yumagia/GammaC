#ifndef PORTAL_GENERATOR_INCLUDED
#define PORTAL_GENERATOR_INCLUDED

#include "GammaFile.hpp"
#include "Bsp.hpp"

#include "Math.hpp"

#include <vector>

class PortalGenerator {
	public:
		void GeneratePortals(BspNode *node);
		
	private:
		void GeneratePortals_r(BspNode *node);
		void CreateNodePortals(BspNode *node);
		void AddPortalToNodes(std::shared_ptr<BspPortal> portal, BspNode *front, BspNode *back);
};

#endif