#include "GradWriter.hpp"
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

	if(!bspFile) {
		exit(1);
	}

	RadiosityBaker baker;
	int numLumels = baker.BakeRad(bspFile);

	GradWriter *bspWriter = new GradWriter(bspFile);
	bspWriter->WriteLevel(argv[1]);

	delete bspWriter;
}