#include "GBSP.h"

#include <vector>
#include <map>


class BspMap {
public: 
	BspMap(bool bspValid) : m_bspValid(bspValid) {}
	~BspMap();

	void Init();

	BspHeader					header;
	
	BspEntities								entities;
	
	std::vector<BspVertex>					vertices;
	std::vector<BspEdge>					edges;
	std::vector<BspFace>					faces;
	std::vector<BspFaceEdge> 				faceEdges;

	std::vector<BspTexInfo>					texInfos;
	std::vector<std::map<std::string, int>>	texIDs;

	std::vector<int>						lightMapData;

	std::vector<BspNode>					nodes;
	std::vector<BspPlane>					planes;
	std::vector<BspLeaf>					leafs;
	std::vector<BspLeafFace> 				leafFaces;

	std::vector<int>						visData;
	std::vector<BspVisOffset>				visOffsets;
protected:
	bool		m_bspValid;
};