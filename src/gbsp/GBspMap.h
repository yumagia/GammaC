#ifndef GBSP_MAP_INCLUDED
#define GBSP_MAP_INCLUDED

#include "GBspStructs.h"

enum LUMPS {
	VERTICES_LUMP,
	PLANES_LUMP,
	FACES_LUMP,
	NODES_LUMP,
	LEAFS_LUMP,
	MODELS_LUMP
};

class GBspMap {
public:
	GBspMap();
	~GBspMap();

	BspHeader	header;

	BspVertex	*vertices = NULL;
	BspPlane	*planes = NULL;
	BspFace		*faces = NULL;

	BspNode		*nodes = NULL;
	BspLeaf		*leafs = NULL;

	BspModel	*models = NULL;
private: 

};


#endif