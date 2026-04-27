#include "FileReader.hpp"

#include "GammaFile.hpp"
#include "GammaDir.hpp"
#include "Atlas.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

int main(int argc, char *argv[]) {
	std::cout << "GRAK Wavefront OBJ Generation Util" << std::endl;

	if(argc < 3) {
		std::cerr << "Program use: ./ObjGen <map name> <output file name>" << std::endl;
		exit(1);
	}

	FileReader bspReader; 
	BspFile *bspFile = bspReader.ReadFile(argv[1]);

	if(!bspFile) {
		exit(1);
	}


	std::ofstream outputFile(OBJ_MESHES_DIR + (std::string) "/" + argv[2] + ".obj");

	if(outputFile.is_open()) {
		std::cout << "Writing to OBJ File..." << std::endl;

		std::vector<std::string> outputFaces;
		std::vector<float> outputVerts;
		std::vector<float> outputUvs;

		Atlas *atlas = new Atlas;
		atlas->Initialize();

		for(int i = 0; i < bspFile->fileHeader.lumps[LUMP_FACES].length; i++) {
			FileFace *face = &bspFile->fileFaces[i];
			std::stringstream faceStream;

			faceStream << "f";

			unsigned int lmOffset = face->lightMapOffset;
			unsigned int lmWidth = face->lightMapWidth;
			unsigned int lmHeight = face->lightMapHeight;

			std::vector<unsigned char> texture;
			texture.reserve(lmWidth * lmHeight * 4);
			for(int j = 0; j < lmHeight; j++) {
				for(int i = 0; i < lmWidth; i++) {
					FileLumel *lumel = &bspFile->fileLightmaps[lmOffset + i + lmWidth * j];
					unsigned int texLoc = (i + lmWidth * j) * 4;
					texture[texLoc] = (unsigned char) (lumel->color[0] * 255.f);
					texture[texLoc + 1] = (unsigned char) (lumel->color[1] * 255.f);
					texture[texLoc + 2] = (unsigned char) (lumel->color[2] * 255.f);
					texture[texLoc + 3] = 255;
				}
			}

			if(!atlas->UploadTexture(lmWidth, lmHeight, texture.data())) {
				std::cerr << "WARNING: Terminating GenerateLightmapAtlas early" << std::endl;
				return 1;
			}

			for(int j = 0; j < face->numVerts; j++) {
				FileVert *vert = &bspFile->fileVerts[face->firstVert + j];

				faceStream << " ";
				faceStream << outputVerts.size() / 3  + 1;
				faceStream << "/";
				faceStream << outputUvs.size() / 2  + 1;

				outputVerts.push_back(vert->point[0]);
				outputVerts.push_back(vert->point[1]);
				outputVerts.push_back(vert->point[2]);

				float &u = vert->lightMapUV[0];
				float &v = vert->lightMapUV[1];
				atlas->RemapUv(u, v);

				outputUvs.push_back(u);
				outputUvs.push_back(v);
			}

			outputFaces.push_back(faceStream.str());
		}

		int vertIndex = 0;
		while(vertIndex < outputVerts.size()) {
			outputFile << "v ";

			for(int i = 0; i < 3; i++) {
				outputFile << outputVerts[vertIndex] << " ";
				vertIndex++;
			}

			outputFile << std::endl;
		}

		int uvIndex = 0;
		while(uvIndex < outputUvs.size()) {
			outputFile << "vt ";

			for(int i = 0; i < 2; i++) {
				outputFile << outputUvs[uvIndex] << " ";
				uvIndex++;
			}

			outputFile << std::endl;
		}

		// baseTexture is the name for the single texture atlas encompassing the gamemap
		outputFile << "usemtl baseTexture" << std::endl;

		for(std::string outputFace : outputFaces) {
			outputFile << outputFace << std::endl;
		}

		atlas->WriteImageFile();

		delete atlas;

		outputFile.close();

		std::cout << "Successfully wrote Wavefront OBJ!" << std::endl;
	}

	std::ofstream mtlFile(OBJ_MESHES_DIR + (std::string) "/" + argv[2] + ".mtl");

	if(mtlFile.is_open()) {
		std::cout << "Writing MTL File..." << std::endl;

		mtlFile << "newmtl baseTexture" << std::endl;
		mtlFile << "Ka 1.000000 1.000000 1.000000" << std::endl;
		mtlFile << "Kd 1.000000 1.000000 1.000000" << std::endl;
		mtlFile << "Ks 0.500000 0.500000 0.500000" << std::endl;
		mtlFile << "Ke 0.000000 0.000000 0.000000" << std::endl;
		mtlFile << "Ni 1.000000" << std::endl;
		mtlFile << "d 1.000000" << std::endl;
		mtlFile << "illum 2" << std::endl;
		mtlFile << "map_Kd atlas.png" << std::endl;

		mtlFile.close();

		std::cout << "Successfully wrote MTL file!" << std::endl;
	}

	delete bspFile;
}