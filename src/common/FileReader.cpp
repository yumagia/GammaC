#include "FileReader.hpp"
#include "GammaFile.hpp"
#include "GammaDir.hpp"

#include <iostream>
#include <fstream>

void FileReader::ReadFile(std::string fileName) {
	std::cout << numModels << "Reading BSP file..." << std::endl;
	BspFile bspFile;

	std::ifstream readFile(fileName);

	std::string line;
	std::getline(readFile, line);

	if(line != "GBSP") {
		std::cerr << "Warning: Incorrect identifier" << std::endl;
	}

	std::getline(readFile, line);
	std::getline(readFile, line);
	numModels = stoi(line) / 12;
	std::cout << numModels << " number of models" << std::endl;

	std::getline(readFile, line);
	std::getline(readFile, line);
	numEntities = stoi(line) / 7;
	std::cout << numEntities << " number of entities" << std::endl;

	std::getline(readFile, line);
	std::getline(readFile, line);
	numPlanes = stoi(line) / 4;
	std::cout << numPlanes << " number of planes" << std::endl;

	std::getline(readFile, line);
	std::getline(readFile, line);
	numNodes = stoi(line) / 11;
	std::cout << numNodes << " number of nodes" << std::endl;

	std::getline(readFile, line);
	std::getline(readFile, line);
	numLeafs = stoi(line) / 9;
	std::cout << numLeafs << " number of leafs" << std::endl;

	std::getline(readFile, line);
	std::getline(readFile, line);
	numLeafFaces = stoi(line);
	std::cout << numLeafFaces << " number of leafFaces" << std::endl;

	std::getline(readFile, line);
	std::getline(readFile, line);
	numVerts = stoi(line) / 3;
	std::cout << numVerts << " number of verts" << std::endl;

	std::getline(readFile, line);
	std::getline(readFile, line);
	numFaceVerts = stoi(line);
	std::cout << numFaceVerts << " number of faceVerts" << std::endl;

	std::getline(readFile, line);
	std::getline(readFile, line);
	numFaces = stoi(line);
	std::cout << numFaces << " number of faces" << std::endl;
}