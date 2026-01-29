#ifndef FILE_READER_INCLUDED
#define FILE_READER_INCLUDED

#include "GammaFile.hpp"

#include <string>

class FileReader {
public:
	BspFile *ReadFile(std::string fileName);

	int			numModels = 0;
	int			numEntities = 0;
	int			numPlanes = 0;
	int			numNodes = 0;
	int			numLeafs = 1;
	int			numLeafFaces = 0;
	int			numVerts = 0;
	int			numFaceVerts = 0;
	int			numFaces = 0;
	int			numMaterials = 0;
	int			numLumels = 0;
	
};

#endif