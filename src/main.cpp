#include <iostream>
#include "gbsp/BspGen.h"
#include "gbsp/MeshLoader.h"
#include "render/Application.h"

int main() {
    std::cout << "GRAK" << std::endl;

    MeshLoader loader;
    LazyMesh *mesh = loader.ParseMeshFile("mesh-files/mesh1.txt");

    Model model;
    model.CreateTreeFromLazyMesh(*mesh);

    Application app;

    return app.Run();
}