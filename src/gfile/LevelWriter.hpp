#include "Bsp.hpp"
#include "GammaFile.h"

class LevelWriter {
public:
	LevelWriter() {}
	~LevelWriter() {}

	void WriteLevel();
	void AddWorldModel(BspModel *model);

	int			numModels = 1;
	int			numEntities = 0;
	int			numPlanes = 0;
	int			numNodes = 0;
	int			numLeafs = 1;
	int			numVerts = 1;
	int			numEdges = 1;
	int			numFaces = 0;

private:
	int EmitTree(BspNode *node);
private:
    int startLeaf;
    int startEdge;
    int startFace;

	FileModel	fileModels[MAX_MAP_MODELS];
	FileEntity	fileEntities[MAX_MAP_ENTITIES];
	FilePlane	filePlanes[MAX_MAP_PLANES];
	FileNode	fileNodes[MAX_MAP_NODES];
	FileLeaf	fileLeafs[MAX_MAP_LEAFS];
	FileVert	fileVerts[MAX_MAP_VERTS];
	FileEdge	fileEdges[MAX_MAP_EDGES];
	FileFace	fileFaces[MAX_MAP_FACES];
};