#ifndef FILEWRITER_INCLUDED
#define FILEWRITER_INCLUDED

#include "GammaFile.hpp"

#include <string>

class FileWriter {
public:
	FileWriter();
	virtual ~FileWriter();
	
	FileWriter(BspFile *bspfile);

	void WriteLevel(std::string fileName);

	int			numModels = 1;
	int			numEntities = 0;
	int			numPlanes = 0;
	int			numNodes = 0;
	int			numLeafs = 2;
	int			numLeafFaces = 0;
	int			numVerts = 0;
	int			numFaceVerts = 0;
	int			numFaces = 0;
	int			numMaterials = 0;
	int			numLumels = 0;

	BspFile	*bspFile;
};

#endif