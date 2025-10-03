#include "GBSPLoader.h"
#include <string.h>

BspMap *GBspLoader::Load(std::string filename) {
    std::ifstream bspFile(filename);

    if(!bspFile.is_open()) {
        return new BspMap(false);
    }

    // Bsp header
    BspHeader bspHeader;
    LoadBspHeader(bspHeader, bspFile);

    // Validate it
    bool validBsp = !strncmp(bspHeader.magic, "IBSP", 4) && (bspHeader.version == 38);

    if(!validBsp) {
        return new BspMap(false);
    }

    BspMap *gammaMap = new BspMap(true);

    gammaMap->header = bspHeader;

    // Entities lump
    LoadEntitiesLump(gammaMap, bspFile);
    // Generic
    LoadLump(gammaMap, Planes, gammaMap->planes, bspFile);
    LoadLump(gammaMap, Vertices, gammaMap->vertices, bspFile);

    LoadLump(gammaMap, Nodes, gammaMap->nodes, bspFile);
    LoadLump(gammaMap, TexInfos, gammaMap->texInfos, bspFile);
    LoadLump(gammaMap, Faces, gammaMap->faces, bspFile);
    LoadLump(gammaMap, Lightmaps, gammaMap->lightMapData, bspFile);
    LoadLump(gammaMap, Leaves, gammaMap->leafs, bspFile);
    LoadLump(gammaMap, LeafFaces, gammaMap->leafFaces, bspFile);
    LoadLump(gammaMap, Edges, gammaMap->edges, bspFile);
    LoadLump(gammaMap, FaceEdges, gammaMap->faceEdges, bspFile);

    LoadVisDataLump(gammaMap, bspFile);

    bspFile.close();

    return gammaMap;
}

void GBspLoader::LoadBspHeader(BspHeader &hdr, std::ifstream &bsp) {
    bsp.read((char*)&(hdr), sizeof(BspHeader));
}

void GBspLoader::LoadEntitiesLump(BspMap *map, std::ifstream &bsp) {
    map->entities.size  = map->header.lump[Entities].length;
    map->entities.ents = new char[map->entities.size];

    bsp.seekg(map->header.lump[Entities].offset, std::ios_base::beg);
    bsp.read(map->entities.ents, sizeof(char) * map->entities.size);
}

void GBspLoader::LoadVisDataLump(BspMap *map, std::ifstream &bsp) {


    bsp.seekg(map->header.lump[VisData].offset, std::ios_base::beg);

    bsp.read((char*)&(map->visData), sizeof(int));
    
}