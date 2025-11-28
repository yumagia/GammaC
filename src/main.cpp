#include <iostream>
#include "Bsp.hpp"
#include "MeshLoader.hpp"
#include "Application.hpp"
#include "BspWriter.hpp"


int main() {
    std::cout << "GRAK" << std::endl;

    FileWriter fileWriter;
    
    fileWriter.BeginBspFile();

    MeshLoader loader;
    LazyMesh *mesh = loader.ParseMeshFile("mesh-files/2brush.obj");

    BspModel model;
    model.CreateTreeFromLazyMesh(mesh);

    fileWriter.AddWorldModel(&model);

    fileWriter.EndBspFile();

    fileWriter.WriteLevel("BspTest.txt");

    Application app;

    return app.Run();
}