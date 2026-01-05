#include "FileWriter.hpp"
#include "FileReader.hpp"

#include "RadiosityBaker.hpp"

#include <iostream>

int main(int argc, char *argv[]) {
	std::cout << "GRAK GRAD" << std::endl;

	if(argc < 2) {
		std::cerr << "Error: Must provide a level BSP file" << std::endl;
		exit(1);
	}

	FileReader bspReader; 
	BspFile *bspFile = bspReader.ReadFile(argv[1]);

	RadiosityBaker baker;
	baker.BakeRad(bspFile);

	FileWriter *bspWriter = new FileWriter(bspFile);
	bspWriter->WriteLevel(argv[1]);

	delete bspWriter;
}