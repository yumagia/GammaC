#include "Bsp.hpp"
#include "GammaFile.hpp"

LazyMesh::~LazyMesh() {
	for(BspVertex *vert : vertexList) {
		if(vert != nullptr) {
			delete vert;
		}
		
		vert = nullptr;
	}

	vertexList.clear();
}