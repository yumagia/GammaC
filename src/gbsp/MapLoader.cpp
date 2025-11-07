#include "MapLoader.h"
#include <fstream>

void MapLoader::LoadMapFile(std::string inputPath) {
	std::cout << "--- LOADING MAP FILE ---" << std::endl;

	inputFile.open(inputPath);
	if(!inputFile.is_open()) {
		std::cerr << "Error: could not open file" << std::endl;
		exit(1);
	}

	while(ParseMapEntity()) {
	}
}

bool	MapLoader::ParseMapEntity(void) {
	Entity		*mapEntity;
	EntityPair	*entPair;
	Side		*side;

	std::string token;
	if(!(inputFile >> token)) {
		return false;
	}
	if(token != "{") {
		std::cerr << "ParseMapEntity Error: { not found" << std::endl;
		exit(1);
	}
}