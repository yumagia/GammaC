#include <iostream>
#include "Bsp.hpp"
#include "MeshLoader.hpp"
#include "GbspWriter.hpp"
#include "GammaDir.hpp"

int main(int argc, char *argv[]) {
    std::cout << "GRAK GBSP" << std::endl;

    if(argc < 3) {
        std::cout << "Program use: ./GBSP <map name> <output file name>" << std::endl;
    }

    GbspWriter *fileWriter = new GbspWriter();
    fileWriter->WriteMap(argv[1], argv[2]);
    delete fileWriter;
}