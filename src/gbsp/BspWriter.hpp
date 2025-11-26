#ifndef LEVEL_WRITER_INCLUDED
#define LEVEL_WRITER_INCLUDED

#include "Bsp.hpp"
#include "GammaFile.h"

#include <string>

class FileWriter {
public:
	FileWriter();
	~FileWriter() {}

	void BeginBspFile();
	void AddWorldModel(BspModel *model);
	void EndBspFile();
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

private:
	int		EmitTree(BspNode *node);
	// -1 denotes air leaf, positive integers are solid
	int		EmitLeaf(BspNode *node);
	void	EmitFace(BspFace *f);
	void	EmitPlanes();
	void	EmitVerts();
	
private:
    int startLeaf;
    int startFaceVert;
    int startFace;

	BspFile	bspFile;
};

#endif