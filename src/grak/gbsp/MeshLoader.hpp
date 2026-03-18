#ifndef MAP_LOADER_INCLUDED
#define MAP_LOADER_INCLUDED

#include "Bsp.hpp"
#include "GammaFile.hpp"

#include <fstream>
#include <map>

// Loader class for mesh
class MeshLoader {
public:	
	MeshLoader() {}

	LazyMesh *ParseMeshFile(const char *fileName, std::map<std::string, int> &materialMap);
	void	ParseMeshFileAppend(LazyMesh *mesh, const char *fileName, std::map<std::string, int> &materialMap);
	int		AddMaterials(const char *fileName, BspFile &bspFile, std::map<std::string, int> &materialMap);

	void	SetContentFlag(int contentFlag);
private:
	std::vector<std::string> ParseArgsFromLine(std::string line);
	void ApplyArgsToMesh(std::vector<std::string> args, LazyMesh *mesh);

	int materialCount;

	int contentFlag;
};

#endif