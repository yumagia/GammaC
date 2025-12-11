#ifndef MAP_LOADER_INCLUDED
#define MAP_LOADER_INCLUDED

#include "Bsp.hpp"
#include "GammaFile.hpp"

#include <fstream>
#include <map>

// Loader class for mesh
class MeshLoader {
public:	
	LazyMesh *ParseMeshFile(const char *fileName, std::map<std::string, int> &materialMap);

	int		AddMaterials(const char *fileName, BspFile &bspFile, std::map<std::string, int> &materialMap);
private:
	std::vector<std::string> ParseArgsFromLine(std::string line);

	int materialCount, vertexCount, faceCount;

	void ApplyArgsToMesh(std::vector<std::string> args, LazyMesh *mesh);
};

#endif