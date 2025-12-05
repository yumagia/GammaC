#include "FileReader.hpp"
#include "GammaFile.hpp"
#include "GammaDir.hpp"

#include <iostream>
#include <fstream>

BspFile *FileReader::ReadFile(std::string fileName) {
	std::cout << "Reading BSP file..." << std::endl;
	BspFile *bspFile = new BspFile();

	std::ifstream readFile(OUTPUT_FILES_DIR + (std::string) "/" + fileName);

	if(!readFile.is_open()) {
		std::cerr << "Error opening file" << std::endl;
		return NULL;
	}

	std::cout << "Successfully opened: " << fileName << std::endl;

	std::string line;
	std::getline(readFile, line);

	if(line != "GBSP") {
		std::cerr << "Warning: Incorrect identifier" << std::endl;
	}

	FileHeader header;

	std::getline(readFile, line);
	header.lumps[LUMP_MODELS].offset = stoi(line);
	std::getline(readFile, line);
	header.lumps[LUMP_MODELS].length = stoi(line);
	numModels = stoi(line) / 12;
	std::cout << "	" << numModels << " number of models" << std::endl;

	std::getline(readFile, line);
	header.lumps[LUMP_ENTITIES].offset = stoi(line);
	std::getline(readFile, line);
	header.lumps[LUMP_ENTITIES].length = stoi(line);
	numEntities = stoi(line) / 7;
	std::cout << "	" << numEntities << " number of entities" << std::endl;

	std::getline(readFile, line);
	header.lumps[LUMP_PLANES].offset = stoi(line);
	std::getline(readFile, line);
	header.lumps[LUMP_PLANES].length = stoi(line);
	numPlanes = stoi(line) / 4;
	std::cout << "	" << numPlanes << " number of planes" << std::endl;

	std::getline(readFile, line);
	header.lumps[LUMP_NODES].offset = stoi(line);
	std::getline(readFile, line);
	header.lumps[LUMP_NODES].length = stoi(line);
	numNodes = stoi(line) / 11;
	std::cout << "	" << numNodes << " number of nodes" << std::endl;

	std::getline(readFile, line);
	header.lumps[LUMP_LEAFS].offset = stoi(line);
	std::getline(readFile, line);
	header.lumps[LUMP_LEAFS].length = stoi(line);
	numLeafs = stoi(line) / 9;
	std::cout << "	" << numLeafs << " number of leafs" << std::endl;

	std::getline(readFile, line);
	header.lumps[LUMP_LEAFFACES].offset = stoi(line);
	std::getline(readFile, line);
	header.lumps[LUMP_LEAFFACES].length = stoi(line);
	numLeafFaces = stoi(line);
	std::cout << "	" << numLeafFaces << " number of leafFaces" << std::endl;

	std::getline(readFile, line);
	header.lumps[LUMP_VERTS].offset = stoi(line);
	std::getline(readFile, line);
	header.lumps[LUMP_VERTS].length = stoi(line);
	numVerts = stoi(line) / 3;
	std::cout << "	" << numVerts << " number of verts" << std::endl;

	std::getline(readFile, line);
	header.lumps[LUMP_FACE_VERTS].offset = stoi(line);
	std::getline(readFile, line);
	header.lumps[LUMP_FACE_VERTS].length = stoi(line);
	numFaceVerts = stoi(line);
	std::cout << "	" << numFaceVerts << " number of faceVerts" << std::endl;

	std::getline(readFile, line);
	header.lumps[LUMP_FACES].offset = stoi(line);
	std::getline(readFile, line);
	header.lumps[LUMP_FACES].length = stoi(line);
	numFaces = stoi(line) / 3;
	std::cout << "	" << numFaces << " number of faces" << std::endl;

	bspFile->fileHeader = header;

	std::getline(readFile, line);
	for(int i = 0; i < numModels; i++) {
		FileModel readModel;

		std::getline(readFile, line);
		readModel.origin[0] = stof(line);
		std::getline(readFile, line);
		readModel.origin[1] = stof(line);
		std::getline(readFile, line);
		readModel.origin[2] = stof(line);

		std::getline(readFile, line);
		readModel.headNode = stoi(line);

		std::getline(readFile, line);
		readModel.firstFace = stoi(line);
		std::getline(readFile, line);
		readModel.numFaces = stoi(line);

		std::getline(readFile, line);
		readModel.minBound[0] = stof(line);
		std::getline(readFile, line);
		readModel.minBound[1] = stof(line);
		std::getline(readFile, line);
		readModel.minBound[2] = stof(line);

		std::getline(readFile, line);
		readModel.maxBound[0] = stof(line);
		std::getline(readFile, line);
		readModel.maxBound[1] = stof(line);
		std::getline(readFile, line);
		readModel.maxBound[2] = stof(line);

		bspFile->fileModels[i] = readModel;
	}

	std::getline(readFile, line);
	for(int i = 0; i < numEntities; i++) {
		FileEntity readEntity;

		std::getline(readFile, line);
		readEntity.origin[0] = stof(line);
		std::getline(readFile, line);
		readEntity.origin[1] = stof(line);
		std::getline(readFile, line);
		readEntity.origin[2] = stof(line);

		std::getline(readFile, line);
		readEntity.model = stoi(line);

		std::getline(readFile, line);
		readEntity.type = stoi(line);

		std::getline(readFile, line);
		readEntity.firstKey = stoi(line);
		std::getline(readFile, line);
		readEntity.numKeys = stoi(line);

		bspFile->fileEntities[i] = readEntity;
	}

	std::getline(readFile, line);
	for(int i = 0; i < numPlanes; i++) {
		FilePlane readPlane;

		std::getline(readFile, line);
		readPlane.normal[0] = stof(line);
		std::getline(readFile, line);
		readPlane.normal[1] = stof(line);
		std::getline(readFile, line);
		readPlane.normal[2] = stof(line);

		std::getline(readFile, line);
		readPlane.dist = stof(line);

		bspFile->filePlanes[i] = readPlane;
	}

	std::getline(readFile, line);
	for(int i = 0; i < numNodes; i++) {
		FileNode readNode;

		std::getline(readFile, line);
		readNode.planeNum = stoi(line);
		std::getline(readFile, line);
		readNode.children[0] = stoi(line);
		std::getline(readFile, line);
		readNode.children[1] = stoi(line);

		std::getline(readFile, line);
		readNode.firstFace = stoi(line);
		std::getline(readFile, line);
		readNode.numFaces = stoi(line);

		std::getline(readFile, line);
		readNode.minBound[0] = stof(line);
		std::getline(readFile, line);
		readNode.minBound[1] = stof(line);
		std::getline(readFile, line);
		readNode.minBound[2] = stof(line);

		std::getline(readFile, line);
		readNode.maxBound[0] = stof(line);
		std::getline(readFile, line);
		readNode.maxBound[1] = stof(line);
		std::getline(readFile, line);
		readNode.maxBound[2] = stof(line);

		bspFile->fileNodes[i] = readNode;
	}

	std::getline(readFile, line);
	for(int i = 0; i < numLeafs; i++) {
		FileLeaf readLeaf;

		std::getline(readFile, line);
		readLeaf.firstLeafFace = stoi(line);
		std::getline(readFile, line);
		readLeaf.numLeafFaces = stoi(line);

		std::getline(readFile, line);
		readLeaf.minBound[0] = stof(line);
		std::getline(readFile, line);
		readLeaf.minBound[1] = stof(line);
		std::getline(readFile, line);
		readLeaf.minBound[2] = stof(line);

		std::getline(readFile, line);
		readLeaf.maxBound[0] = stof(line);
		std::getline(readFile, line);
		readLeaf.maxBound[1] = stof(line);
		std::getline(readFile, line);
		readLeaf.maxBound[2] = stof(line);

		std::getline(readFile, line);
		readLeaf.visOffset = stoi(line);

		bspFile->fileLeafs[i] = readLeaf;
	}

	std::getline(readFile, line);
	for(int i = 0; i < numLeafFaces; i++) {
		int readLeafFace;

		std::getline(readFile, line);
		readLeafFace = stoi(line);

		bspFile->fileLeafFaces[i] = readLeafFace;
	}

	std::getline(readFile, line);
	for(int i = 0; i < numVerts; i++) {
		FileVert readVert;

		std::getline(readFile, line);
		readVert.point[0] = stof(line);
		std::getline(readFile, line);
		readVert.point[1] = stof(line);
		std::getline(readFile, line);
		readVert.point[2] = stof(line);

		bspFile->fileVerts[i] = readVert;
	}

	std::getline(readFile, line);
	for(int i = 0; i < numFaceVerts; i++) {
		int readFaceVert;

		std::getline(readFile, line);
		readFaceVert = stoi(line);

		bspFile->fileFaceVerts[i] = readFaceVert;
	}

	std::getline(readFile, line);
	for(int i = 0; i < numFaces; i++) {
		FileFace readFace;

		std::getline(readFile, line);
		readFace.firstVert = stoi(line);
		std::getline(readFile, line);
		readFace.numVerts = stoi(line);
		std::getline(readFile, line);
		readFace.planeNum = stoi(line);

		bspFile->fileFaces[i] = readFace;
	}

	readFile.close();
	std::cout << "Successful read!" << std::endl;
	bspFile->valid = true;

	return bspFile;
}