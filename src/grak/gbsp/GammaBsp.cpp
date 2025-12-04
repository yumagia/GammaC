#include <iostream>
#include "Bsp.hpp"
#include "MeshLoader.hpp"
#include "BspWriter.hpp"

int main() {
    std::cout << "GRAK GBSP" << std::endl;

    FileWriter fileWriter;
    
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