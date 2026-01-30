#include "GradWriter.hpp"

GradWriter::GradWriter(BspFile *bspFile) {
    FileHeader header = bspFile->fileHeader;

	numModels = header.lumps[LUMP_MODELS].length;
	numEntities = header.lumps[LUMP_ENTITIES].length;
	numPlanes = header.lumps[LUMP_PLANES].length;
	numNodes = header.lumps[LUMP_NODES].length;
	numLeafs = header.lumps[LUMP_LEAFS].length;
	numLeafFaces = header.lumps[LUMP_LEAFFACES].length;
	numVerts = header.lumps[LUMP_VERTS].length;
	numFaceVerts = header.lumps[LUMP_FACE_VERTS].length;
	numFaces = header.lumps[LUMP_FACES].length;
	numMaterials = header.lumps[LUMP_MATERIALS].length;
	numLumels = header.lumps[LUMP_LUMELS].length;
    
    this->bspFile = bspFile;
}