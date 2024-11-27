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
