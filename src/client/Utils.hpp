#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <fstream>
#include <unordered_map>

namespace Color {
    const std::string RESET   = "\033[0m";
    const std::string RED     = "\033[31m";
    const std::string GREEN   = "\033[32m";
    const std::string YELLOW  = "\033[33m";
    const std::string BLUE    = "\033[34m";
}

namespace GameAction {
    const std::string NONE        = "0000";  // 0
    const std::string SHOOT       = "0001";  // 1
    const std::string MOVE        = "00010";  // 2
    const std::string DOWN        = "0011";  // 3
    const std::string LEFT        = "0100";  // 4
    const std::string RIGHT       = "0101";  // 5
    const std::string UP          = "0110";  // 6
    const std::string CROUCH      = "0111";  // 7
    const std::string POWER_UP    = "1000";  // 8
    const std::string SHIELD      = "1001";  // 9
    const std::string RESPAWN     = "1010";  // 10
    const std::string DISCONNECT  = "11011"; // 27
    const std::string CONNECT     = "11111"; // 31
    const std::string QUIT        = "1111";   // 15
};

class Utils {
    public:
        Utils() = default;
        ~Utils() = default;
        sf::Sprite cat(const std::string& path);
        void printTerminal(std::string color, std::string mess);
        void printLog(std::string mess);

    private:

};
