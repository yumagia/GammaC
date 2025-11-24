#ifndef LEVEL_WRITER_INCLUDED
#define LEVEL_WRITER_INCLUDED

#include "Bsp.hpp"
#include "GammaFile.h"

class FileWriter {
public:
	FileWriter();
	~FileWriter() {}

	void WriteLevel();
	void AddWorldModel(BspModel *model);

	int			numModels = 1;
	int			numEntities = 0;
	int			numPlanes = 0;
	int			numNodes = 0;
	int			numLeafs = 1;
	int			numLeafFaces = 1;
	int			numVerts = 1;
	int			numFaceVerts = 1;
	int			numFaces = 0;

private:
	int		EmitTree(BspNode *node);
	void	EmitLeaf(BspNode *node);
	void	EmitFace(BspFace *f);
	
private:
    int startLeaf;
    int startFaceVert;
    int startFace;

	BspFile	*bspFile;
};

#endif