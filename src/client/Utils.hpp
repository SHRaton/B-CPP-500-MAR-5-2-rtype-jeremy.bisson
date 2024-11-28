#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <fstream>
#include <unordered_map>
#pragma once

namespace Color {
    const std::string RESET   = "\033[0m";
    const std::string RED     = "\033[31m";
    const std::string GREEN   = "\033[32m";
    const std::string YELLOW  = "\033[33m";
    const std::string BLUE    = "\033[34m";
}
class Utils {
    public:
        Utils() = default;
        ~Utils() = default;
        sf::Sprite cat(const std::string& path);
        void printTerminal(std::string color, std::string mess);
        void printLog(std::string mess);

    private:

};
