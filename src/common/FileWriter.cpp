#include "FileWriter.hpp"
#include "GammaFile.hpp"
#include "GammaDir.hpp"

#include <iostream>
#include <fstream>
#include <filesystem>

FileWriter::FileWriter() {
	numModels = 1;		// Model 0 is reserved for the world
	numEntities = 0;
	numPlanes = 0;
	numNodes = 0;
	numLeafs = 2;		// Leaf 0 is non-negatable. Leaf as error. Leaf 1 is solid
	numVerts = 0;
	numFaceVerts = 0;
	numFaces = 0;
}

void FileWriter::WriteLevel(std::string fileName) {
	std::cout << "--- WriteLevel ---" << std::endl;

	std::cout << "Writing first-pass file..." << std::endl;
	
	FileHeader header;

	std::ofstream outputFile(fileName);

	int numLines = 19;
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
		

		outputFile << "FACEVERTS LUMP" << std::endl;
		numLines++;
		header.lumps[LUMP_FACE_VERTS].offset = numLines;
		for(int i = 0; i < numFaceVerts; i++) {
			outputFile << bspFile.fileFaceVerts[i] << std::endl;
			numLines ++;
		}
		header.lumps[LUMP_FACE_VERTS].length = numLines - header.lumps[LUMP_FACE_VERTS].offset;
		
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
	for(int i = 0; i < NUM_LUMPS; i++) {
		secondPass << header.lumps[i].offset << std::endl;
		secondPass << header.lumps[i].length << std::endl;
	}

	std::string line;
	while(std::getline(original, line)) {
		secondPass << line << std::endl;
	}

	try {
		std::filesystem::remove(fileName);
		std::filesystem::rename("temp" + fileName, OUTPUT_FILES_DIR + (std::string) "/" + fileName);
		std::cout << "File update success!" << std::endl;
	} catch (const std::filesystem::filesystem_error& e) {
		std::cerr << "Filesystem error: " << e.what() << std::endl;
	}

	std::cout << "Successfully wrote: " << fileName << std::endl;
}