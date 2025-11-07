#include "GBsp.h"
#include "MapLoader.h"

int GBsp::Run(int argc, char **argv) {
    std::cout << "--- RUNNING GBSP ---" << std::endl;

    int i;
	for(i = 1; i < argc; i++) {
		std::string argument = argv[i];

	}

	if(i != argc - 1) {
		std::cerr << "Error! Usage is: gbsp [arguments] mapfile" << std::endl;
	}

    mapLoader.LoadMapFile(fileName);
}


int main(int argc, char **argv) {
    GBsp gBsp;

    gBsp.Run(argc, argv);
};