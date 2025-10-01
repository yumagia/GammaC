#include "GBSP.h"

#include <vector>
#include <map>


class BspMap {
public: 
	BspMap(bool bspValid) : m_bspValid(bspValid) {}
	~BspMap();

	void Init();

	BspHeader					header;
	
	BspVertex					vertices;
	BspEdge						edges;
	BspFace						faces;
	BspFaceEdge 				faceEdges;

	BspTexInfo					texInfos;
	std::map<std::string, int>	texIDs;

	int							lightMapData;

	BspNode						nodes;
	BspPlane					planes;
	BspLeaf						leafs;
	BspLeafFace 				leafFaces;

	int							VisData;
	BspVisOffset				VisOffsets;
protected:
	bool		m_bspValid;
};