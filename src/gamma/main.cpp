#include "Application.hpp"

#include <iostream>

int main(int argv, char **argc) {
    std::cout << "--- GAMMA ENGINE ---" << std::endl;

    if(argv < 2) {
        std::cerr << "Usage is: ./GAMMA <bsp file name>" << std::endl;
        exit(1);
    }

    Application app;
    app.Initialize();
    
    std::string fileName = argc[1];
    app.ReadBspFile(fileName);

    app.Run();

	exit(0);
}