#include "GBSPMap.h"
#include <fstream>

class GBspLoader {
public:
	BspMap *Load(std::string filename);

private:
	void LoadBspHeader(BspHeader &hdr,  std::ifstream &bsp);
	void LoadEntitiesLump(BspMap *map, std::ifstream &bsp);
	void LoadVisDataLump(BspMap *map, std::ifstream &bsp);

	template<class T>
	void LoadLump(BspMap *map, LumpTypes lType, std::vector<T> &container, std::ifstream &bsp);
};

// Common loader for generic bsp lumps
template<class T>
void GBspLoader::LoadLump(BspMap *map, LumpTypes lType, std::vector<T> &container, std::ifstream &bsp) {
	int numElements = map->header.lump[lType].length / sizeof(T);
	bsp.seekg(map->header.lump[lType].offset, std::ios_base::beg);

	container.reserve(numElements);

	for (int i = 0; i < numElements; i++) {
		T element;
		bsp.read((char*)&element, sizeof(T));

		container.push_back(element);
	}
}