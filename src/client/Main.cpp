#include "Core.hpp"
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc != 1 && argc != 3) {
        std::cerr << "Usage: ./client" << std::endl;
        return 1;
    }
    Core core;
    if (argc == 3) {
        core.login_auto(std::string(argv[1]), std::string(argv[2]));
    } else if (argc == 1) {
        core.gui(argc, argv);
    }
    return 0;
}
