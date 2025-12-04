#include <iostream>
#include "Bsp.hpp"
#include "MeshLoader.hpp"
#include "GbspWriter.hpp"

int main(int argc, char *argv[]) {
    std::cout << "GRAK GBSP" << std::endl;

    GbspWriter fileWriter;
    
    fileWriter.BeginBspFile();

    MeshLoader loader;
    LazyMesh *mesh = loader.ParseMeshFile("mesh-files/blockout.obj");

    BspModel model;
    model.CreateTreeFromLazyMesh(mesh);
    delete mesh;

    fileWriter.AddWorldModel(&model);

    FreeTree(model.root);

    fileWriter.EndBspFile();

    fileWriter.WriteLevel("blockout.txt");
}