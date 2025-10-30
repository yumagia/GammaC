#include "MeshLoader.h"
#include "Bsp.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>

int MeshLoader::ApplyArgsToMesh(std::vector<std::string> args, LazyMesh *mesh) {
	switch(currentState) {
	case BASE_STATE:
		if(!(args.empty() || args[0][0] == '#')) {
			if(args[0] == "s") {            // Set solid flag
				mesh->solid = (bool) stoi(args[1]);
				std::cout << "Set solidity to " << mesh->solid << std::endl;
			}
			if(args[0] == "o") {            // Parse object
				std::cout << "Reading Mesh..." << std::endl;
				return COUNTS_STATE;
			}
		}
		return BASE_STATE;

	case COUNTS_STATE:
		materialCount = stoi(args[0]);
		vertexCount = stoi(args[1]);
		mesh->vertexList.reserve(vertexCount);
		faceCount = stoi(args[2]);
		mesh->faces.reserve(faceCount);
		counter = 0;
		std::cout << "\tMaterial count: " << materialCount << std::endl;
		std::cout << "\tVertex count: " << vertexCount << std::endl;
		std::cout << "\tFace count: " << faceCount << std::endl;
		return MATERIALS_STATE;

	case MATERIALS_STATE:
		counter++;
		if(counter > materialCount) {
			counter = 0;
			return VERTICES_STATE;
		}
		// TODO: Apply material name
		return MATERIALS_STATE;

	case VERTICES_STATE:
		counter++;
		if(counter > vertexCount) {
			counter = 0;
			return FACES_STATE;
		}
		mesh->vertexList.push_back(new BspVertex(stof(args[0]), stof(args[1]), stof(args[2])));
		return VERTICES_STATE;

	case FACES_STATE:
		counter++;
		if(counter > faceCount) {
			counter = 0;
			return BASE_STATE;
		}
		int numVerts = stoi(args[0]);
		Vec3f verts[numVerts];
		for(int i = 0; i < numVerts; i++) {
			verts[i] = mesh->vertexList[stoi(args[i + 1])]->point;
		}
		mesh->faces.push_back(new BspFace(numVerts, verts, PlaneFromTriangle(verts[0], verts[1], verts[2])));
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
	std::cout << newMesh->vertexList.size() << " New vertices" << std::endl;
	std::cout << newMesh->faces.size() << " New faces" << std::endl;


	return newMesh;
}
