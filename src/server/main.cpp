#include "ServerNetwork.hpp"
#include "ServerGame.hpp"
#include <iostream>

#ifdef _WIN32
#include <windows.h>
void enableANSI() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
}
#endif

int main(int argc, char* argv[]) {
    #ifdef _WIN32
    enableANSI();
    #endif
    srand(time(NULL));
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <port>\n";
        return 1;
    }

    try {
        Mediator mediator;
        ServerNetwork server(std::stoi(argv[1]), mediator);
        ServerGame game(mediator);
        server.start();
    }
    catch (std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    return 0;
}