#include "Bsp.hpp"
#include "GammaFile.h"

LazyMesh::~LazyMesh() {
    for(BspVertex* vert : vertexList) {
        delete vert;
    }

    vertexList.clear();
}