#include "client.hpp"
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cout << "Usage: " << argv[0] << " <server_ip> <port>\n";
        return 1;
    }

    try {
        ChatClient client(argv[1], std::stoi(argv[2]));
        client.start();
    }
    catch (std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    return 0;
}