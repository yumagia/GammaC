#include <iostream>
#include "mapping/Bsp.h"
#include "mapping/MeshLoader.h"

int main() {
    std::cout << "GRAK" << std::endl;

    MeshLoader loader;
    LazyMesh *mesh = loader.ParseMeshFile("../mesh-files/mesh1.txt");

    return 0;
}