#include "GBSPMap.h"
#include <fstream>

class GBspLoader {
public:
	BspMap *Load(std::string filename);

private:
	void LoadBspHeader(BspHeader &hdr,  std::ifstream &bsp);
	void LoadEntitiesLump(BspMap *map, std::ifstream &bsp);
	void LoadVisDataLump(BspMap *map, std::ifstream &bsp);

	void LoadLump(BspMap *map, LumpTypes lType, std::ifstream &bsp);
};