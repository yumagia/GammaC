#include <iostream>
#include "Bsp.hpp"
#include "MeshLoader.hpp"
#include "GbspWriter.hpp"

int main(int argc, char *argv[]) {
    std::cout << "GRAK GBSP" << std::endl;

    if(argc < 3) {
        std::cout << "Program use: ./GBSP <.obj file> <output file name>" << std::endl;
    }

    GbspWriter fileWriter;
    
    fileWriter.BeginBspFile();

    MeshLoader loader;
    LazyMesh *mesh = loader.ParseMeshFile(argv[1]);

    BspModel model;
    model.CreateTreeFromLazyMesh(mesh);
    delete mesh;

    fileWriter.AddWorldModel(&model);

    FreeTree(model.root);

    fileWriter.EndBspFile();

    fileWriter.WriteLevel(((std::string) argv[2]) + ".txt");
}