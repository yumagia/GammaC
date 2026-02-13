#include "FileReader.hpp"

#include "GammaFile.hpp"
#include "GammaDir.hpp"

#include <iostream>
#include <fstream>

int main(int argc, char *argv[]) {
	std::cout << "GRAK VBO Generation Util" << std::endl;

	if(argc < 3) {
		std::cerr << "Program use: ./VboGen <map name> <output file name>" << std::endl;
		exit(1);
	}

	FileReader bspReader; 
	BspFile *bspFile = bspReader.ReadFile(argv[1]);

	if(!bspFile) {
		exit(1);
	}

	std::cout << "Writing to vertex mesh File..." << std::endl;

    std::ofstream outputFile(VERTEX_MESHES_DIR + (std::string) "/" + argv[2]);

    if(outputFile.is_open()) {
		for(int i = 0; i < bspFile->fileHeader.lumps[LUMP_FACES].length; i++) {
			FileFace *face = &bspFile->fileFaces[i];

			for(int j = 0; j < face->numVerts; j++) {
				FileVert *vert = &bspFile->fileVerts[face->firstVert + j];

				outputFile << vert->point[0]/100.f << ", " << vert->point[1]/100.f << ", " << vert->point[2]/100.f << ", " << std::endl;
			}

			outputFile << std::endl;
		}

        outputFile.close();
    }

	std::cout << "Successfully wrote vertex mesh file!" << std::endl;

	delete bspFile;
}