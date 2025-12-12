#include "Application.hpp"

#include <iostream>

int main(int argv, char **argc) {
    if(argv < 2) {
        std::cout << "Usage is: ./GAMMA <bsp file name>" << std::endl;
        exit(1);
    }

    Application app;
    app.Initialize();

    std::string fileName = argc[1];
    app.ReadBspFile(fileName);

    app.Run();

    std::cout << "Sucessfully closed application" << std::endl;
}