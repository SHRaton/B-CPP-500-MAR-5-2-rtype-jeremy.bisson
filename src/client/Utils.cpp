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

std::string Utils::keyToString(sf::Keyboard::Key key, bool shift)
{
    // Gestion des touches spéciales d'abord
    switch (key) {
        case sf::Keyboard::Up: return "Up";
        case sf::Keyboard::Down: return "Down";
        case sf::Keyboard::Left: return "Left";
        case sf::Keyboard::Right: return "Right";
        case sf::Keyboard::Return: return "Enter";
        case sf::Keyboard::BackSpace: return "Backspace";
        case sf::Keyboard::Delete: return "Delete";
        case sf::Keyboard::Insert: return "Insert";
        case sf::Keyboard::Home: return "Home";
        case sf::Keyboard::End: return "End";
        case sf::Keyboard::PageUp: return "Page Up";
        case sf::Keyboard::PageDown: return "Page Down";
        case sf::Keyboard::Escape: return "Escape";
        default: {
            // Pour les touches standard, utilise keyToAscii
            char asciiChar = keyToAscii(key, shift);
            if (asciiChar != '\0') {
                return std::string(1, asciiChar);
            }
            return "Unknown";
        }
    }
}

char Utils::keyToAscii(sf::Keyboard::Key key, bool shift)
{
    if (key >= sf::Keyboard::A && key <= sf::Keyboard::Z) {
        return shift ? 'A' + (key - sf::Keyboard::A) : 'a' + (key - sf::Keyboard::A);
    } else if (key >= sf::Keyboard::Num0 && key <= sf::Keyboard::Num9) {
        return '0' + (key - sf::Keyboard::Num0);
    } else if (key >= sf::Keyboard::Numpad0 && key <= sf::Keyboard::Numpad9) {
        return '0' + (key - sf::Keyboard::Numpad0);
    } else {
        switch (key) {
            case sf::Keyboard::Space: return ' ';
            case sf::Keyboard::Tab: return '\t';
            case sf::Keyboard::Comma: return shift ? '?' : ',';
            case sf::Keyboard::SemiColon: return shift ? '.' : ';';
            case sf::Keyboard::Slash: return shift ? '/' : ':';
            case sf::Keyboard::Period: return shift ? ':' : '.';
            case sf::Keyboard::BackSlash: return shift ? '|' : '\\';
            case sf::Keyboard::Tilde: return shift ? '~' : '`';
            case sf::Keyboard::Equal: return shift ? '+' : '=';
            case sf::Keyboard::Dash: return shift ? '_' : '-';
            case sf::Keyboard::LBracket: return shift ? '{' : '[';
            case sf::Keyboard::RBracket: return shift ? '}' : ']';
            case sf::Keyboard::Quote: return shift ? '"' : '\'';
            default: return '\0';
        }
    }
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
