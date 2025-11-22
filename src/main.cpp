#include <iostream>
#include "gbsp/Bsp.hpp"
#include "gbsp/MeshLoader.hpp"
#include "render/Application.hpp"

int main() {
    std::cout << "GRAK" << std::endl;

    MeshLoader loader;
    LazyMesh *mesh = loader.ParseMeshFile("mesh-files/mesh1.txt");

    BspModel model;
    model.CreateTreeFromLazyMesh(*mesh);

    Application app;

    return app.Run();
}