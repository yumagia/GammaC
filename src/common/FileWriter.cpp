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

	numMaterials = 0;
}

FileWriter::~FileWriter() {
	delete bspFile;
	bspFile = NULL;
}

FileWriter::FileWriter(BspFile *bspFile) {
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

void FileWriter::WriteLevel(std::string fileName) {
	std::cout << "--- WriteLevel ---" << std::endl;

	std::cout << "Writing first-pass file..." << std::endl;
	
	FileHeader header = bspFile->fileHeader;

	std::ofstream outputFile(fileName);

	int numLines = 19;
	if(outputFile.is_open()) {
		outputFile << "MODELS LUMP" << std::endl;
		numLines++;
		header.lumps[LUMP_MODELS].offset = numLines;
		for(int i = 0; i < numModels; i++) {
			FileModel *currentModel = &bspFile->fileModels[i];

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
		header.lumps[LUMP_MODELS].length = numModels;
		
		outputFile << "ENTITIES LUMP" << std::endl;
		numLines++;
		header.lumps[LUMP_ENTITIES].offset = numLines;
		for(int i = 0; i < numEntities; i++) {
			FileEntity *currentEntity = &bspFile->fileEntities[i];

			outputFile << currentEntity->origin[0] << std::endl;
			outputFile << currentEntity->origin[1] << std::endl;
			outputFile << currentEntity->origin[2] << std::endl;

			outputFile << currentEntity->model << std::endl;

			outputFile << currentEntity->type << std::endl;

			outputFile << currentEntity->firstKey << std::endl;
			outputFile << currentEntity->numKeys << std::endl;

			numLines += 7;
		}
		header.lumps[LUMP_ENTITIES].length = numEntities;

		outputFile << "PLANES LUMP" << std::endl;
		numLines++;
		header.lumps[LUMP_PLANES].offset = numLines;
		for(int i = 0; i < numPlanes; i++) {
			FilePlane *currentPlane = &bspFile->filePlanes[i];

			outputFile << currentPlane->type << std::endl;

			outputFile << currentPlane->normal[0] << std::endl;
			outputFile << currentPlane->normal[1] << std::endl;
			outputFile << currentPlane->normal[2] << std::endl;

			outputFile << currentPlane->dist << std::endl;

			numLines += 5;
		}
		header.lumps[LUMP_PLANES].length = numPlanes;

		outputFile << "NODES LUMP" << std::endl;
		numLines++;
		header.lumps[LUMP_NODES].offset = numLines;
		for(int i = 0; i < numNodes; i++) {
			FileNode *currentNode = &bspFile->fileNodes[i];

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
		header.lumps[LUMP_NODES].length = numNodes;

		outputFile << "LEAFS LUMP" << std::endl;
		numLines++;
		header.lumps[LUMP_LEAFS].offset = numLines;
		for(int i = 0; i < numLeafs; i++) {
			FileLeaf *currentLeaf = &bspFile->fileLeafs[i];

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
		header.lumps[LUMP_LEAFS].length = numLeafs;

		outputFile << "LEAF FACES LUMP" << std::endl;
		numLines++;
		header.lumps[LUMP_LEAFFACES].offset = numLines;
		for(int i = 0; i < numLeafFaces; i++) {
			outputFile << bspFile->fileLeafFaces[i] << std::endl;
			numLines++;
		}
		header.lumps[LUMP_LEAFFACES].length = numLeafFaces;

		outputFile << "VERTS LUMP" << std::endl;
		numLines++;
		header.lumps[LUMP_VERTS].offset = numLines;
		for(int i = 0; i < numVerts; i++) {
			FileVert *currentVert = &bspFile->fileVerts[i];

			outputFile << currentVert->point[0] << std::endl;
			outputFile << currentVert->point[1] << std::endl;
			outputFile << currentVert->point[2] << std::endl;

			outputFile << currentVert->normal[0] << std::endl;
			outputFile << currentVert->normal[1] << std::endl;
			outputFile << currentVert->normal[2] << std::endl;

			outputFile << currentVert->surfaceUV[0] << std::endl;
			outputFile << currentVert->surfaceUV[1] << std::endl;

			outputFile << currentVert->lightMapUV[0] << std::endl;
			outputFile << currentVert->lightMapUV[1] << std::endl;

			numLines += 10;
		}
		header.lumps[LUMP_VERTS].length = numVerts;
		

		outputFile << "FACEVERTS LUMP" << std::endl;
		numLines++;
		header.lumps[LUMP_FACE_VERTS].offset = numLines;
		for(int i = 0; i < numFaceVerts; i++) {
			outputFile << bspFile->fileFaceVerts[i] << std::endl;
			numLines ++;
		}
		header.lumps[LUMP_FACE_VERTS].length = numFaceVerts;
		
		outputFile << "FACES LUMP" << std::endl;
		numLines++;
		header.lumps[LUMP_FACES].offset = numLines;
		for(int i = 0; i < numFaces; i++) {
			FileFace *currentFace = &bspFile->fileFaces[i];

			outputFile << currentFace->firstVert << std::endl;
			outputFile << currentFace->numVerts << std::endl;
			outputFile << currentFace->planeNum << std::endl;
			outputFile << currentFace->material << std::endl;

			outputFile << currentFace->lightMapOffset << std::endl;
			outputFile << currentFace->lightMapWidth << std::endl;
			outputFile << currentFace->lightMapHeight << std::endl;

			outputFile << currentFace->lightMapOrigin[0] << std::endl;
			outputFile << currentFace->lightMapOrigin[1] << std::endl;
			outputFile << currentFace->lightMapOrigin[2] << std::endl;

			outputFile << currentFace->lightMapS[0] << std::endl;
			outputFile << currentFace->lightMapS[1] << std::endl;
			outputFile << currentFace->lightMapS[2] << std::endl;

			outputFile << currentFace->lightMapT[0] << std::endl;
			outputFile << currentFace->lightMapT[1] << std::endl;
			outputFile << currentFace->lightMapT[2] << std::endl;

			numLines += 16;
		}
		header.lumps[LUMP_FACES].length = numFaces;

		outputFile << "MATERIALS LUMP" << std::endl;
		numLines++;
		header.lumps[LUMP_MATERIALS].offset = numLines;
		for(int i = 0; i < numMaterials; i++) {
			FileMaterial *currentMaterial = &bspFile->fileMaterials[i];

			outputFile << currentMaterial->diffuse[0] << std::endl;
			outputFile << currentMaterial->diffuse[1] << std::endl;
			outputFile << currentMaterial->diffuse[2] << std::endl;

			outputFile << currentMaterial->specular[0] << std::endl;
			outputFile << currentMaterial->specular[1] << std::endl;
			outputFile << currentMaterial->specular[2] << std::endl;

			outputFile << currentMaterial->emissive[0] << std::endl;
			outputFile << currentMaterial->emissive[1] << std::endl;
			outputFile << currentMaterial->emissive[2] << std::endl;

			outputFile << currentMaterial->specCoeff << std::endl;

			numLines += 10;
		}
		header.lumps[LUMP_MATERIALS].length = numMaterials;

		outputFile << "LIGHTMAP LUMP" << std::endl;
		numLines++;
		header.lumps[LUMP_LUMELS].offset = numLines;
		for(int i = 0; i < numLumels; i++) {
			FileLumel *currentLumel = &bspFile->fileLightmaps[i];

			outputFile << currentLumel->legal << std::endl;

			numLines += 1;
		}
		header.lumps[LUMP_LUMELS].length = numLumels;
		
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