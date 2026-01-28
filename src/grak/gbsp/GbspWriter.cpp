#include "GbspWriter.hpp"

#include "Bsp.hpp"
#include "GammaFile.hpp"
#include "GammaDir.hpp"

#include "MeshLoader.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <map>

extern	int			numMapPlanes;
extern	BspPlane 	mapPlanes[MAX_MAP_PLANES];

extern	int			numMapVerts;
extern	BspVertex 	mapVerts[MAX_MAP_VERTS];
extern	int			numMapFaceVerts;

GbspWriter::~GbspWriter() {

}

void GbspWriter::WriteMap(const char *mapDir, const char *bspLevelName) {
	std::string expMapDir = MAPS_DIR + (std::string) "/" + mapDir + "/";
	std::ifstream scriptsFile(expMapDir + (std::string) "scripts.txt");
	
	BeginBspFile();

	std::string line;
	std::vector<std::string> args;
	while(std::getline(scriptsFile, line)) {
		args = ParseArgsFromLine(line);

		if(args[0] == "wmodel") {
			MeshLoader meshLoader;

			std::string mtlFileName = expMapDir + "mesh-files/" + args[1] + ".mtl";
			numMaterials += meshLoader.AddMaterials(mtlFileName.c_str(), *bspFile, materialMap);

			std::string objFileName = expMapDir + "mesh-files/" + args[1] + ".obj";
			LazyMesh *mesh = meshLoader.ParseMeshFile(objFileName.c_str(), materialMap);

			if(!mesh) {
				return;
			}

			BspModel model;
			model.CreateTreeFromLazyMesh(mesh);
			delete mesh;

			AddWorldModel(&model);

			FreeTree(model.root);
		}
	}

	EndBspFile();
	
    WriteLevel(((std::string) bspLevelName) + ".txt");
}

std::vector<std::string> GbspWriter::ParseArgsFromLine(std::string line) {
	std::vector<std::string> args;

	std::istringstream iss(line);
	std::string arg;
	while(iss >> arg) {
		args.push_back(arg);
	}

	return args;
}

void GbspWriter::BeginBspFile() {
	std::cout << "--- Initialize BSP File ---" << std::endl; 
	numMapPlanes = 0;
	numMapVerts = 0;
	numMapFaceVerts = 0;

	bspFile = new BspFile();
}

void GbspWriter::AddWorldModel(BspModel *model) {
	std::cout << "--- AddWorldModel ---" << std::endl;
	std::cout << "Setting world model..." << std::endl;

	bspFile->fileModels[0].firstFace = numFaces;

	bspFile->fileModels[0].minBound[0] = model->bounds.min.x;
	bspFile->fileModels[0].minBound[1] = model->bounds.min.y;
	bspFile->fileModels[0].minBound[2] = model->bounds.min.z;
	bspFile->fileModels[0].maxBound[0] = model->bounds.max.x;
	bspFile->fileModels[0].maxBound[1] = model->bounds.max.y;
	bspFile->fileModels[0].maxBound[2] = model->bounds.max.z;

	std::cout << "Outputting tree to file..." << std::endl;
	bspFile->fileModels[0].headNode = EmitTree(model->root);

	bspFile->fileModels[0].numFaces = numFaces - bspFile->fileModels[0].firstFace;
	std::cout << "Successfully outputted world model!" << std::endl;
}

int GbspWriter::EmitLeaf(BspNode *node) {
	FileLeaf *emittedLeaf;

	if(node->faces.empty()) {
		return 1;
	}

	if(numLeafs >= MAX_MAP_LEAFS) {
		std::cerr << "Reached MAX_MAP_LEAFS: " << MAX_MAP_LEAFS << std::endl;
	}

	emittedLeaf = &bspFile->fileLeafs[numLeafs];
	numLeafs++;

	emittedLeaf->minBound[0] = node->bounds.min.x - BOUND_PADDING;
	emittedLeaf->minBound[1] = node->bounds.min.y - BOUND_PADDING;
	emittedLeaf->minBound[2] = node->bounds.min.z - BOUND_PADDING;
	emittedLeaf->maxBound[0] = node->bounds.max.x + BOUND_PADDING;
	emittedLeaf->maxBound[1] = node->bounds.max.y + BOUND_PADDING;
	emittedLeaf->maxBound[2] = node->bounds.max.z + BOUND_PADDING;

	emittedLeaf->firstLeafFace = numLeafFaces;

	for(std::shared_ptr<BspFace> face : node->faces) {
		int faceNum = face->outputNumber;
		if(numLeafFaces >= MAX_MAP_LEAF_FACES) {
			std::cerr << "Reached MAX_MAP_LEAF_FACES: " << MAX_MAP_LEAF_FACES << std::endl;
		}
		bspFile->fileLeafFaces[numLeafFaces] = faceNum;
		numLeafFaces++;
	}

	emittedLeaf->numLeafFaces = numLeafFaces - emittedLeaf->firstLeafFace;

	return -1;
}

void GbspWriter::EmitFace(std::shared_ptr<BspFace> face) {
	FileFace *emittedFace;
	
	face->outputNumber = numFaces;

	if(numFaces >= MAX_MAP_FACES) {
		std::cerr << "Reached MAX_MAP_FACES: " << MAX_MAP_FACES << std::endl;
	}

	emittedFace = &bspFile->fileFaces[numFaces];
	numFaces++;

	emittedFace->planeNum = face->planeNum;
	emittedFace->material = face->materialNum;
	
	emittedFace->firstVert = numFaceVerts;
	int numVerts = face->vertIndices.size();
	emittedFace->numVerts = numVerts;
	for(int i = 0; i < numVerts; i++) {
		bspFile->fileFaceVerts[numFaceVerts] = face->vertIndices[i];
		numFaceVerts++;
	}
}

int GbspWriter::EmitTree(BspNode *node) {
	
	if(node->isLeaf) {
		if(EmitLeaf(node) > 0) {
			return 0;
		}
		return -numLeafs;
	}

	if(numNodes >= MAX_MAP_NODES) {
		std::cerr << "Reached MAX_MAP_NODES: " << MAX_MAP_NODES << std::endl;
	}
	
	FileNode *emittedNode;
	emittedNode = &bspFile->fileNodes[numNodes];
	numNodes++;

	emittedNode->minBound[0] = node->bounds.min.x - BOUND_PADDING;
	emittedNode->minBound[1] = node->bounds.min.y - BOUND_PADDING;
	emittedNode->minBound[2] = node->bounds.min.z - BOUND_PADDING;
	emittedNode->maxBound[0] = node->bounds.max.x + BOUND_PADDING;
	emittedNode->maxBound[1] = node->bounds.max.y + BOUND_PADDING;
	emittedNode->maxBound[2] = node->bounds.max.z + BOUND_PADDING;

	emittedNode->planeNum = node->planeNum;
	emittedNode->firstFace = numFaces;
	for(std::shared_ptr<BspFace> face : node->faces) {
		EmitFace(face);
	}

	emittedNode->numFaces = numFaces - emittedNode->firstFace;

	if(node->back->isLeaf) {
		emittedNode->children[0] = -numLeafs;
		if(EmitLeaf(node->back) > 0) {
			emittedNode->children[0] = -1;
		}
	}
	else {
		emittedNode->children[0] = numNodes;
		EmitTree(node->back);
	}

	if(node->front->isLeaf) {
		emittedNode->children[1] = -numLeafs;
		if(EmitLeaf(node->front) > 0) {
			emittedNode->children[0] = -1;
		}
	}
	else {
		emittedNode->children[1] = numNodes;
		EmitTree(node->front);
	}

	return emittedNode - bspFile->fileNodes;
}

void GbspWriter::EmitPlanes() {
	for(int i = 0; i < numMapPlanes; i++) {
		FilePlane *emittedPlane = &bspFile->filePlanes[numPlanes];
		numPlanes++;

		emittedPlane->type = mapPlanes[i].type;
		
		emittedPlane->normal[0] = mapPlanes[i].normal.x;
		emittedPlane->normal[1] = mapPlanes[i].normal.y;
		emittedPlane->normal[2] = mapPlanes[i].normal.z;

		emittedPlane->dist = mapPlanes[i].dist;		
	}
}

void GbspWriter::EmitVerts() {
	for(int i = 0; i < numMapVerts; i++) {
		FileVert *emittedVert = &bspFile->fileVerts[numVerts];
		numVerts++;

		emittedVert->point[0] = mapVerts[i].point.x;
		emittedVert->point[1] = mapVerts[i].point.y;
		emittedVert->point[2] = mapVerts[i].point.z;
	}
}

void GbspWriter::EndBspFile() {
	std::cout << "--- End BSP File ---" << std::endl;

	FileLeaf *errorLeaf = &bspFile->fileLeafs[0];
	errorLeaf->firstLeafFace = 0;
	errorLeaf->numLeafFaces = 0;
	errorLeaf->minBound[0] = 0;
	errorLeaf->minBound[1] = 0;
	errorLeaf->minBound[2] = 0;
	errorLeaf->maxBound[0] = 0;
	errorLeaf->maxBound[1] = 0;
	errorLeaf->maxBound[2] = 0;

	FileLeaf *solidLeaf = &bspFile->fileLeafs[1];
	solidLeaf->firstLeafFace = 0;
	solidLeaf->numLeafFaces = 0;
	solidLeaf->minBound[0] = 0;
	solidLeaf->minBound[1] = 0;
	solidLeaf->minBound[2] = 0;
	solidLeaf->maxBound[0] = 0;
	solidLeaf->maxBound[1] = 0;
	solidLeaf->maxBound[2] = 0;

	EmitPlanes();
	EmitVerts();

	FileHeader header;

	header.lumps[LUMP_MODELS].length = numModels;
	header.lumps[LUMP_ENTITIES].length = numEntities;
	header.lumps[LUMP_PLANES].length = numPlanes;
	header.lumps[LUMP_NODES].length = numNodes;
	header.lumps[LUMP_LEAFS].length = numLeafs;
	header.lumps[LUMP_LEAFFACES].length = numLeafFaces;
	header.lumps[LUMP_VERTS].length = numVerts;
	header.lumps[LUMP_FACE_VERTS].length = numFaceVerts;
	header.lumps[LUMP_FACES].length = numFaces;

	bspFile->fileHeader = header;

	bspFile->valid = true;

	std::cout << "Successfully Validated BSP file" << std::endl;
}