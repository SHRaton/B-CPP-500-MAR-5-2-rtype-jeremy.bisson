#include "Core.hpp"
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc != 1) {
        std::cerr << "Usage: ./client" << std::endl;
        return 1;
    }
    Core core;
    core.gui(argc, argv);
    return 0;
}
