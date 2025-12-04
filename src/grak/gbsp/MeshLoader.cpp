#include "MeshLoader.hpp"
#include "GammaFile.hpp"
#include "Bsp.hpp"

#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <memory>

void MeshLoader::ApplyArgsToMesh(std::vector<std::string> args, LazyMesh *mesh) {
	if(args[0] == "v") {
		mesh->vertexList.push_back(new BspVertex(stof(args[1]), stof(args[2]), stof(args[3])));
	}

	if(args[0] == "f") {
		int numVerts = args.size() - 1;

		std::vector<int> vertIndices;
		for(int i = 0; i < numVerts; i++) {
			std::stringstream arg(args[i + 1]);
			std::string vert;
			std::getline(arg, vert, '/');
			vertIndices.push_back(stoi(vert) - 1);
		}

		std::shared_ptr<BspFace> newFace = std::make_shared<BspFace>(vertIndices, PlaneNumFromTriangle(mesh->vertexList[vertIndices[0]]->point, mesh->vertexList[vertIndices[1]]->point, mesh->vertexList[vertIndices[2]]->point));

		mesh->faces.push_back(newFace);
	}
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

		ApplyArgsToMesh(args, newMesh);
	}

	file.close();
	std::cout << "Sucessfully created new lazy mesh" << std::endl;
	std::cout << "\t" << newMesh->faces.size() << " New faces" << std::endl;
	std::cout << "\t" << newMesh->vertexList.size() << " New vertices" << std::endl;
	std::cout << std::endl;

	return newMesh;
}
