#include "FileWriter.hpp"
#include "FileReader.hpp"

#include <iostream>

int main(int argc, char *argv[]) {
    std::cout << "GRAK GRAD" << std::endl;

    if(argc < 2) {
        std::cerr << "Error: Must provide a level BSP file" << std::endl;
    }

    FileReader bspReader; 
    BspFile *bspFile = bspReader.ReadFile(argv[1]);

    delete bspFile;
}