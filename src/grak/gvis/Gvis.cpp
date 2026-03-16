#include "FileWriter.hpp"
#include "FileReader.hpp"

#include <iostream>

int main(int argc, char *argv[]) {
	std::cout << "GRAK GVIS" << std::endl;

	if(argc < 2) {
		std::cerr << "Error: Must provide a level BSP file" << std::endl;
		exit(1);
	}

	FileReader bspReader; 
	BspFile *bspFile = bspReader.ReadFile(argv[1]);

	if(!bspFile) {
		exit(1);
	}


	FileWriter *bspWriter = new FileWriter(bspFile);
	bspWriter->WriteLevel(argv[1]);

	delete bspWriter;
}