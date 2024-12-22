#include "Utils.hpp"

sf::Sprite Utils::cat(const std::string& path)
{
    static std::map<std::string, sf::Texture> textureCache;
    if (textureCache.find(path) == textureCache.end()) {
        sf::Texture texture;
        if (!texture.loadFromFile(path))
        {
            throw std::runtime_error("Failed to load texture: " + path);
        }
        textureCache[path] = texture;
    }
    sf::Sprite sprite;
    sprite.setTexture(textureCache[path]);
    return sprite;
}

void Utils::setOriginToMiddle(sf::Sprite& sprite)
{
    sf::FloatRect bounds = sprite.getLocalBounds();
    sprite.setOrigin(bounds.width/2, bounds.height/2);
}

void Utils::printTerminal(std::string color, std::string mess)
{
    std::cout << color << "[Console] : " << mess << Color::RESET << std::endl;
}

void Utils::printLog(std::string mess)
{
    const std::string logFileName = "../src/client/logs/log.txt";
    std::ofstream logFile(logFileName, std::ios::app);

    if (!logFile) {
        std::cerr << "Erreur : Impossible d'ouvrir le fichier de log." << std::endl;
        return;
    }
    std::time_t now = std::time(nullptr);
    char timeBuffer[100];
    std::strftime(timeBuffer, sizeof(timeBuffer), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
    logFile << "[" << timeBuffer << "] " << mess << std::endl;
    logFile.close();
}
