#include "MeshLoader.hpp"
#include "GammaFile.hpp"
#include "Bsp.hpp"

#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <memory>

void MeshLoader::ApplyArgsToMesh(std::vector<std::string> args, LazyMesh *mesh) {

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

int MeshLoader::AddMaterials(const char *fileName, BspFile &bspFile, std::map<std::string, int> &materialMap) {
	std::cout << " --- Adding materials from file ---" << std::endl;
	
	std::ifstream file;
	file.open(fileName);
	std::cout << "Opening: " << fileName << std::endl;

	if(!file.is_open()) {
		std::cerr << "Error opening file!" << std::endl;
		return 0;
	}

	std::string line;
	std::vector<std::string> args;
	FileMaterial *material = NULL;
	while(std::getline(file, line)) {
		args = ParseArgsFromLine(line);

		if(args.empty()) {
			continue;
		}

		if(args[0] == "newmtl") {
			if(materialMap.find(args[1]) == materialMap.end()) {		// Add a new material if not found in map
				materialMap.insert({args[1], materialMap.size()});
				material = &bspFile.fileMaterials[materialCount];
				materialCount++;
				std::cout << "	Added material: " << args[1] << std::endl;
			}
			else {
				material = NULL;
			}
		}
		if(material) {
			if(args[0] == "Kd") {
				material->diffuse[0] = stof(args[1]);
				material->diffuse[1] = stof(args[2]);
				material->diffuse[2] = stof(args[3]);
			}
			else if(args[0] == "Ks") {
				material->specular[0] = stof(args[1]);
				material->specular[1] = stof(args[2]);
				material->specular[2] = stof(args[3]);
			}
			else if(args[0] == "Ke") {
				material->emissive[0] = stof(args[1]);
				material->emissive[1] = stof(args[2]);
				material->emissive[2] = stof(args[3]);
			}
			else if(args[0] == "Ns") {
				material->specCoeff = stof(args[1]);
			}
		}
	}

	file.close();
	std::cout << "Success!" << std::endl;

	return materialCount;
}

LazyMesh *MeshLoader::ParseMeshFile(const char *fileName, std::map<std::string, int> &materialMap) {
	std::cout << " --- Creating new object ---" << std::endl;
	
	std::ifstream file;
	file.open(fileName);
	std::cout << "Opening: " << fileName << std::endl;

	if(!file.is_open()) {
		std::cerr << "Error opening file!" << std::endl;
		return NULL;
	}

	LazyMesh *newMesh = new LazyMesh();

	std::string line;
	std::vector<std::string> args;
	int currMaterialNum;
	while(std::getline(file, line)) {
		args = ParseArgsFromLine(line);

		if(args.empty()) {
			continue;
		}

		if(args[0] == "usemtl") {
			currMaterialNum = materialMap[args[1]];
		}
		
		if(args[0] == "v") {
			newMesh->vertexList.push_back(new BspVertex(stof(args[1]), stof(args[2]), stof(args[3])));
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
			int planeNum =	PlaneNumFromTriangle(	newMesh->vertexList[vertIndices[0]]->point, 
													newMesh->vertexList[vertIndices[1]]->point, 
													newMesh->vertexList[vertIndices[2]]->point	);
			
			std::shared_ptr<BspFace> newFace = std::make_shared<BspFace>(vertIndices, planeNum, TextureFromMaterial(planeNum, currMaterialNum, Vec3f()));

			newMesh->faces.push_back(newFace);
		}
	}

	file.close();
	std::cout << "Successfully created new lazy mesh" << std::endl;
	std::cout << "\t" << newMesh->faces.size() << " New faces" << std::endl;
	std::cout << "\t" << newMesh->vertexList.size() << " New vertices" << std::endl;
	std::cout << std::endl;

	return newMesh;
}

