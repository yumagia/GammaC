#include "GBSPLoader.h"

BspMap *GBspLoader::Load(std::string filename) {
    std::ifstream bspFile(filename);

    if(!bspFile.is_open()) {
        return new BspMap(false);
    }

    // Bsp header
    BspHeader bspHeader;
    LoadBspHeader(bspHeader, bspFile);

    // Validate it
    bool validBsp = (bspHeader.magic == "IBSP") && (bspHeader.version == 38);

    if(!validBsp) {
        return new BspMap(false);
    }

    BspMap *gammaMap = new BspMap(true);

    gammaMap->header = bspHeader;

    // Entities lump

    bspFile.close();

    return gammaMap;
}