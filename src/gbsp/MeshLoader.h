#ifndef MAP_LOADER_INCLUDED
#define MAP_LOADER_INCLUDED

#include "BspGen.h"
#include <fstream>

struct LazyMesh {
	bool	solid;
	std::vector<BspFace*>	faces;
	std::vector<BspVertex*> vertexList;
};

// Loader class for mesh
class MeshLoader {
public:	
	LazyMesh *ParseMeshFile(const char *fileName);

private:
	std::vector<std::string> ParseArgsFromLine(std::string line);

	enum {	BASE_STATE,
			COUNTS_STATE,
			MATERIALS_STATE,
			VERTICES_STATE,
			FACES_STATE			};

	int currentState = BASE_STATE;
	int counter;

	int materialCount, vertexCount, faceCount;

	int ApplyArgsToMesh(std::vector<std::string> args, LazyMesh *mesh);

	std::ifstream file;
	std::string currentLine;
};

#endif