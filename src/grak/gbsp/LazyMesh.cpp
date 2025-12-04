#include "Bsp.hpp"
#include "GammaFile.hpp"

LazyMesh::~LazyMesh() {
    for(BspVertex* vert : vertexList) {
        delete vert;
    }

    vertexList.clear();
}