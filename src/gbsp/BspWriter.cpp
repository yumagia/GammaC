#include "BspWriter.hpp"

#include "Bsp.hpp"
#include "GammaFile.h"

#include <iostream>
#include <fstream>
#include <filesystem>

extern	int			numMapPlanes;
extern	BspPlane 	mapPlanes[MAX_MAP_PLANES];

extern	int			numMapVerts;
extern	BspVertex 	mapVerts[MAX_MAP_VERTS];
extern	int			numMapFaceVerts;


FileWriter::FileWriter() {
	numModels = 1;
	numEntities = 0;
	numPlanes = 0;
	numNodes = 0;
	numLeafs = 1;		// Leaf 0 is non-negatable. Leaf as error
	numVerts = 0;
	numFaceVerts = 0;
	numFaces = 0;
}

void FileWriter::BeginBspFile() {
	std::cout << "--- Initialize BSP ---" << std::endl; 
	numMapPlanes = 0;
	numMapVerts = 0;
	numMapFaceVerts = 0;
}

void FileWriter::AddWorldModel(BspModel *model) {
	std::cout << "--- AddWorldModel ---" << std::endl;
	std::cout << "Setting world model..." << std::endl;

	bspFile.fileModels[0].firstFace = numFaces;

	bspFile.fileModels[0].minBound[0] = model->bounds.min.x;
	bspFile.fileModels[0].minBound[1] = model->bounds.min.y;
	bspFile.fileModels[0].minBound[2] = model->bounds.min.z;
	bspFile.fileModels[0].maxBound[0] = model->bounds.max.x;
	bspFile.fileModels[0].maxBound[1] = model->bounds.max.y;
	bspFile.fileModels[0].maxBound[2] = model->bounds.max.z;

	std::cout << "Outputting tree to file..." << std::endl;
	bspFile.fileModels[0].headNode = EmitTree(model->root);
	std::cout << "Successfully wrote world model!" << std::endl;
}

int FileWriter::EmitLeaf(BspNode *node) {
	FileLeaf *emittedLeaf;

	if(node->faces.empty()) {
		return 1;
	}

	if(numLeafs >= MAX_MAP_LEAFS) {
		std::cerr << "Reached MAX_MAP_LEAFS: " << MAX_MAP_LEAFS << std::endl;
	}

	emittedLeaf = &bspFile.fileLeafs[numLeafs];
	numLeafs++;

	emittedLeaf->minBound[0] = node->bounds.min.x;
	emittedLeaf->minBound[1] = node->bounds.min.y;
	emittedLeaf->minBound[2] = node->bounds.min.z;
	emittedLeaf->maxBound[0] = node->bounds.max.x;
	emittedLeaf->maxBound[1] = node->bounds.max.y;
	emittedLeaf->maxBound[2] = node->bounds.max.z;

	emittedLeaf->firstLeafFace = numLeafFaces;

	for(BspFace *face : node->faces) {
		int faceNum = face->outputNumber;
		if(numLeafFaces >= MAX_MAP_LEAF_FACES) {
			std::cerr << "Reached MAX_MAP_LEAF_FACES: " << MAX_MAP_LEAF_FACES << std::endl;
		}
		bspFile.fileLeafFaces[numLeafFaces] = faceNum;
		numLeafFaces++;
	}

	emittedLeaf->numLeafFaces = numLeafFaces - emittedLeaf->firstLeafFace;

	return -1;
}

void FileWriter::EmitFace(BspFace *face) {
	FileFace *emittedFace;
	
	face->outputNumber = numFaces;

	if(numFaces >= MAX_MAP_FACES) {
		std::cerr << "Reached MAX_MAP_FACES: " << MAX_MAP_FACES << std::endl;
	}

	emittedFace = &bspFile.fileFaces[numFaces];
	numFaces++;

	emittedFace->planeNum = face->planeNum;
	
	emittedFace->firstVert = numFaceVerts;
	int numVerts = face->vertIndices.size();
	emittedFace->numVerts = numVerts;
	for(int i = 0; i < numVerts; i++) {
		bspFile.fileFaceVert[numFaceVerts] = face->vertIndices[i];
		numFaceVerts++;
	}
}

int FileWriter::EmitTree(BspNode *node) {
	
	if(node->isLeaf) {
		if(EmitLeaf(node) > 0) {
			return -1;
		}
		return -numLeafs;
	}

	if(numNodes >= MAX_MAP_NODES) {
		std::cerr << "Reached MAX_MAP_NODES: " << MAX_MAP_NODES << std::endl;
	}
	
	FileNode *emittedNode;
	emittedNode = &bspFile.fileNodes[numNodes];
	numNodes++;

	emittedNode->minBound[0] = node->bounds.min.x;
	emittedNode->minBound[1] = node->bounds.min.y;
	emittedNode->minBound[2] = node->bounds.min.z;
	emittedNode->maxBound[0] = node->bounds.max.x;
	emittedNode->maxBound[1] = node->bounds.max.y;
	emittedNode->maxBound[2] = node->bounds.max.z;

	emittedNode->planeNum = node->planeNum;
	emittedNode->firstFace = numFaces;
	for(BspFace *face : node->faces) {
		EmitFace(face);
	}

	emittedNode->numFaces = numFaces - emittedNode->firstFace;

	if(node->back->isLeaf) {
		emittedNode->children[0] = -(numLeafs + 1);
		EmitLeaf(node->back);
		if(EmitLeaf(node) > 0) {
			emittedNode->children[0] = -1;
		}
	}
	else {
		emittedNode->children[0] = numNodes;
		EmitTree(node->back);
	}

	if(node->front->isLeaf) {
		emittedNode->children[1] = -(numLeafs + 1);
		EmitLeaf(node->front);
		if(EmitLeaf(node) > 0) {
			emittedNode->children[0] = -1;
		}
	}
	else {
		emittedNode->children[1] = numNodes;
		EmitTree(node->front);
	}

	return emittedNode - bspFile.fileNodes;
}

void FileWriter::EmitPlanes() {
	for(int i = 0; i < numMapPlanes; i++) {
		FilePlane *emittedPlane = &bspFile.filePlanes[numPlanes];
		numPlanes++;
		
		emittedPlane->normal[0] = mapPlanes[i].normal.x;
		emittedPlane->normal[1] = mapPlanes[i].normal.y;
		emittedPlane->normal[2] = mapPlanes[i].normal.z;

		emittedPlane->dist = mapPlanes[i].dist;		
	}
}

void FileWriter::EmitVerts() {
	for(int i = 0; i < numMapVerts; i++) {
		FileVert *emittedVert = &bspFile.fileVerts[numVerts];
		numVerts++;

		emittedVert->point[0] = mapVerts[i].point.x;
		emittedVert->point[1] = mapVerts[i].point.y;
		emittedVert->point[2] = mapVerts[i].point.z;
	}
}

void FileWriter::EndBspFile() {
	FileLeaf *solidLeaf = &bspFile.fileLeafs[0];
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
}

void FileWriter::WriteLevel(std::string fileName) {
	std::cout << "--- WriteLevel ---" << std::endl;

	std::cout << "Writing first-pass file..." << std::endl;
	
	FileHeader header;

	std::ofstream outputFile(fileName);

	int numLines = 17;
	if(outputFile.is_open()) {
		outputFile << "MODELS LUMP" << std::endl;
		numLines++;
		header.lumps[LUMP_MODELS].offset = numLines;
		for(int i = 0; i < numModels; i++) {
			FileModel *currentModel = &bspFile.fileModels[i];

			outputFile << currentModel->origin[0] << std::endl;
			outputFile << currentModel->origin[1] << std::endl;
			outputFile << currentModel->origin[2] << std::endl;

			outputFile << currentModel->headNode << std::endl;

			outputFile << currentModel->firstFace << std::endl;
			outputFile << currentModel->numFaces << std::endl;

			outputFile << currentModel->minBound[0] << std::endl;
			outputFile << currentModel->minBound[1] << std::endl;
			outputFile << currentModel->minBound[2] << std::endl;
			outputFile << currentModel->maxBound[0] << std::endl;
			outputFile << currentModel->maxBound[1] << std::endl;
			outputFile << currentModel->maxBound[2] << std::endl;

			numLines += 12;
		}
		header.lumps[LUMP_MODELS].length = numLines - header.lumps[LUMP_MODELS].offset;
		
		outputFile << "ENTITIES LUMP" << std::endl;
		numLines++;
		header.lumps[LUMP_ENTITIES].offset = numLines;
		for(int i = 0; i < numEntities; i++) {
			FileEntity *currentEntity = &bspFile.fileEntities[i];

			outputFile << currentEntity->origin[0] << std::endl;
			outputFile << currentEntity->origin[1] << std::endl;
			outputFile << currentEntity->origin[2] << std::endl;

			outputFile << currentEntity->model << std::endl;

			outputFile << currentEntity->type << std::endl;

			outputFile << currentEntity->firstKey << std::endl;
			outputFile << currentEntity->numKeys << std::endl;

			numLines += 7;
		}
		header.lumps[LUMP_ENTITIES].length = numLines - header.lumps[LUMP_ENTITIES].offset;

		outputFile << "PLANES LUMP" << std::endl;
		numLines++;
		header.lumps[LUMP_PLANES].offset = numLines;
		for(int i = 0; i < numPlanes; i++) {
			FilePlane *currentPlane = &bspFile.filePlanes[i];

			outputFile << currentPlane->normal[0] << std::endl;
			outputFile << currentPlane->normal[1] << std::endl;
			outputFile << currentPlane->normal[2] << std::endl;

			outputFile << currentPlane->dist << std::endl;

			numLines += 4;
		}
		header.lumps[LUMP_PLANES].length = numLines - header.lumps[LUMP_PLANES].offset;

		outputFile << "NODES LUMP" << std::endl;
		numLines++;
		header.lumps[LUMP_NODES].offset = numLines;
		for(int i = 0; i < numNodes; i++) {
			FileNode *currentNode = &bspFile.fileNodes[i];

			outputFile << currentNode->planeNum << std::endl;
			outputFile << currentNode->children[0] << std::endl;
			outputFile << currentNode->children[1] << std::endl;

			outputFile << currentNode->firstFace << std::endl;
			outputFile << currentNode->numFaces << std::endl;

			outputFile << currentNode->minBound[0] << std::endl;
			outputFile << currentNode->minBound[1] << std::endl;
			outputFile << currentNode->minBound[2] << std::endl;
			outputFile << currentNode->maxBound[0] << std::endl;
			outputFile << currentNode->maxBound[1] << std::endl;
			outputFile << currentNode->maxBound[2] << std::endl;

			numLines += 11;
		}
		header.lumps[LUMP_NODES].length = numLines - header.lumps[LUMP_NODES].offset;

		outputFile << "LEAFS LUMP" << std::endl;
		numLines++;
		header.lumps[LUMP_LEAFS].offset = numLines;
		for(int i = 0; i < numLeafs; i++) {
			FileLeaf *currentLeaf = &bspFile.fileLeafs[i];

			outputFile << currentLeaf->firstLeafFace << std::endl;
			outputFile << currentLeaf->numLeafFaces << std::endl;

			outputFile << currentLeaf->minBound[0] << std::endl;
			outputFile << currentLeaf->minBound[1] << std::endl;
			outputFile << currentLeaf->minBound[2] << std::endl;
			outputFile << currentLeaf->maxBound[0] << std::endl;
			outputFile << currentLeaf->maxBound[1] << std::endl;
			outputFile << currentLeaf->maxBound[2] << std::endl;

			outputFile << currentLeaf->visOffset << std::endl;

			numLines += 9;
		}
		header.lumps[LUMP_LEAFS].length = numLines - header.lumps[LUMP_LEAFS].offset;

		outputFile << "LEAF FACES LUMP" << std::endl;
		numLines++;
		header.lumps[LUMP_LEAFFACES].offset = numLines;
		for(int i = 0; i < numLeafFaces; i++) {
			outputFile << bspFile.fileLeafFaces[i] << std::endl;
			numLines++;
		}
		header.lumps[LUMP_LEAFFACES].length = numLines - header.lumps[LUMP_LEAFFACES].offset;

		outputFile << "VERTS LUMP" << std::endl;
		numLines++;
		header.lumps[LUMP_VERTS].offset = numLines;
		for(int i = 0; i < numVerts; i++) {
			FileVert *currentVert = &bspFile.fileVerts[i];

			outputFile << currentVert->point[0] << std::endl;
			outputFile << currentVert->point[1] << std::endl;
			outputFile << currentVert->point[2] << std::endl;

			numLines += 3;
		}
		header.lumps[LUMP_VERTS].length = numLines - header.lumps[LUMP_VERTS].offset;
		
		outputFile << "FACES LUMP" << std::endl;
		numLines++;
		header.lumps[LUMP_FACES].offset = numLines;
		for(int i = 0; i < numFaces; i++) {
			FileFace *currentFace = &bspFile.fileFaces[i];

			outputFile << currentFace->firstVert << std::endl;
			outputFile << currentFace->numVerts << std::endl;
			outputFile << currentFace->planeNum << std::endl;

			numLines += 3;
		}
		header.lumps[LUMP_FACES].length = numLines - header.lumps[LUMP_FACES].offset;
		
		outputFile.close();
	}

	std::cout << "Writing second file with updated header..." << std::endl;

	std::ifstream original(fileName);
	std::ofstream secondPass("temp" + fileName);

	numLines = 0;
	if(!(original.is_open() && secondPass.is_open())) {
		std::cerr << "ERROR OPENING FILES!" << std::endl;
	}

	// OVERWRITE THE HEADER LINES
	secondPass << "GBSP" << std::endl;
	secondPass << header.lumps[LUMP_MODELS].offset << std::endl;
	secondPass << header.lumps[LUMP_MODELS].length << std::endl;
	secondPass << header.lumps[LUMP_ENTITIES].offset << std::endl;
	secondPass << header.lumps[LUMP_ENTITIES].length << std::endl;
	secondPass << header.lumps[LUMP_PLANES].offset << std::endl;
	secondPass << header.lumps[LUMP_PLANES].length << std::endl;
	secondPass << header.lumps[LUMP_NODES].offset << std::endl;
	secondPass << header.lumps[LUMP_NODES].length << std::endl;
	secondPass << header.lumps[LUMP_LEAFS].offset << std::endl;
	secondPass << header.lumps[LUMP_LEAFS].length << std::endl;
	secondPass << header.lumps[LUMP_LEAFFACES].offset << std::endl;
	secondPass << header.lumps[LUMP_LEAFFACES].length << std::endl;
	secondPass << header.lumps[LUMP_VERTS].offset << std::endl;
	secondPass << header.lumps[LUMP_VERTS].length << std::endl;
	secondPass << header.lumps[LUMP_FACES].offset << std::endl;
	secondPass << header.lumps[LUMP_FACES].length << std::endl;

	std::string line;
	while(std::getline(original, line)) {
		secondPass << line << std::endl;
	}

	try {
		std::filesystem::remove(fileName);
		std::filesystem::rename("temp" + fileName, fileName);
		std::cout << "File update success!" << std::endl;
	} catch (const std::filesystem::filesystem_error& e) {
		std::cerr << "Filesystem error: " << e.what() << std::endl;
	}

	std::cout << "Successfully wrote: " << fileName << std::endl;
}