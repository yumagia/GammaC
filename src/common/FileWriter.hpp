#ifndef FILEWRITER_INCLUDED
#define FILEWRITER_INCLUDED

#include "GammaFile.hpp"

#include <string>

class FileWriter {
public:
	FileWriter();
	~FileWriter() {}

	void WriteLevel(std::string fileName);

	int			numModels = 1;
	int			numEntities = 0;
	int			numPlanes = 0;
	int			numNodes = 0;
	int			numLeafs = 1;
	int			numLeafFaces = 0;
	int			numVerts = 0;
	int			numFaceVerts = 0;
	int			numFaces = 0;

	BspFile	bspFile;
};

#endif