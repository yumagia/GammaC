#ifndef MAP_LOADER_INCLUDED
#define MAP_LOADER_INCLUDED

#include "GBsp.h"

#include <iostream>

class MapLoader {
public:
    void LoadMapFile(std::string inputPath);

private:
    bool ParseMapEntity(void) {

    }

private:
    std::ifstream inputFile;
};

#endif