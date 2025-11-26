#include "MeshLoader.hpp"
#include "Bsp.hpp"

#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>

int MeshLoader::ApplyArgsToMesh(std::vector<std::string> args, LazyMesh *mesh) {
	if(args.empty() || args[0][0] == '#') {
		return currentState;
	}
	
	switch(currentState) {
	case BASE_STATE:
		if(args[0] == "s") {            // Set solid flag
			mesh->solid = (bool) stoi(args[1]);
			std::cout << "Set solidity to " << mesh->solid << std::endl;
		}
		if(args[0] == "o") {            // Parse object
			std::cout << "Reading Mesh..." << std::endl;
			return COUNTS_STATE;
		}
		return BASE_STATE;

	case COUNTS_STATE:
		materialCount = stoi(args[0]);
		vertexCount = stoi(args[1]);
		mesh->vertexList.reserve(vertexCount);
		faceCount = stoi(args[2]);
		mesh->faces.reserve(faceCount);
		counter = 1;
		std::cout << "\tMaterial count: " << materialCount << std::endl;
		std::cout << "\tVertex count: " << vertexCount << std::endl;
		std::cout << "\tFace count: " << faceCount << std::endl;

		std::cout << "Reading materials... " << std::endl;
		return MATERIALS_STATE;

	case MATERIALS_STATE:

		// TODO: Apply material name

		counter++;
		if(counter > materialCount) {
			counter = 1;
			std::cout << "Reading verts... " << std::endl;
			return VERTICES_STATE;
		}
		return MATERIALS_STATE;

	case VERTICES_STATE:
		// New vertex
		mesh->vertexList.push_back(new BspVertex(stof(args[0]), stof(args[1]), stof(args[2])));

		counter++;
		if(counter > vertexCount) {
			counter = 1;
			std::cout << "Reading faces... " << std::endl;
			return FACES_STATE;
		}
		return VERTICES_STATE;

	case FACES_STATE:
		// New face
		int numVerts = stoi(args[0]);
		int vertIndices[numVerts];
		for(int i = 0; i < numVerts; i++) {
			vertIndices[i] = stoi(args[i + 1]);
		}
		mesh->faces.push_back(new BspFace(numVerts, vertIndices, PlaneNumFromTriangle(mesh->vertexList[vertIndices[0]]->point, mesh->vertexList[vertIndices[1]]->point, mesh->vertexList[vertIndices[2]]->point)));
		counter++;
		if(counter > faceCount) {
			counter = 1;
			return BASE_STATE;
		}
		return FACES_STATE;
	}
	
	return BASE_STATE;
}

std::vector<std::string> MeshLoader::ParseArgsFromLine(std::string line) {
	std::vector<std::string> args;

	std::istringstream iss(line);
	std::string arg;
	while(iss >> arg) {
		args.push_back(arg);
	}

	return args;
}

LazyMesh *MeshLoader::ParseMeshFile(const char *fileName) {
	std::cout << " --- Creating new object ---" << std::endl;
	
	file.open(fileName);
	std::cout << "Opening: " << fileName << std::endl;

	if(!file.is_open()) {
		std::cerr << "Error opening file!" << std::endl;
		abort();
	}

	LazyMesh *newMesh = new LazyMesh();

	std::vector<std::string> args;
	while(std::getline(file, currentLine)) {
		args = ParseArgsFromLine(currentLine);

		currentState = ApplyArgsToMesh(args, newMesh);
	}

	file.close();
	std::cout << "Sucessfully created new lazy mesh" << std::endl;
	std::cout << "\t" << newMesh->faces.size() << " New faces" << std::endl;
	std::cout << "\t" << newMesh->vertexList.size() << " New vertices" << std::endl;
	std::cout << std::endl;

	return newMesh;
}
