#include <cstddef>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <unordered_map>
#include "Utils.hpp"

class Sprite {
private:
    sf::Sprite sprite;
    sf::Sprite hoverSprite;
    bool isScrolling = false;
    sf::Clock clock;
    float scrollSpeed = 0;
    int updateInterval = 0;
    bool isHoverable = false;
    bool isHovered = false;

public:
    Sprite() = default;
    Sprite(const std::string& texturePath, bool scrolling = false, float speed = 1.0f,
            int interval = 50, const std::string& hoverTexturePath = "") {
        Utils utils;
        sprite = utils.cat(texturePath);
        isScrolling = scrolling;
        scrollSpeed = speed;
        updateInterval = interval;
        if (!hoverTexturePath.empty()) {
            try {
                hoverSprite = utils.cat(hoverTexturePath);
                isHoverable = true;
            } catch (const std::runtime_error& e) {
                std::cerr << "Warning: Hover texture not found: " << hoverTexturePath << std::endl;
                isHoverable = false;
            }
        }
    }

    void update() {
        if (isScrolling && clock.getElapsedTime().asMilliseconds() >= updateInterval) {
            sprite.move(-1, 0);
            if (sprite.getPosition().x <= -sprite.getGlobalBounds().width / 2) {
                sprite.setPosition(0, 0);
            }
            clock.restart();
        }
    }

    void setHovered(bool hover) {
        isHovered = hover;
    }

    bool isMouseOver(const sf::RenderWindow& window) {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        return sprite.getGlobalBounds().contains(mousePos.x, mousePos.y);
    }

    void setScale(sf::Vector2f scale) {
        sprite.setScale(scale);
        if (isHoverable) {
            hoverSprite.setScale(scale);
        }
    }

    void setOriginToMiddle() {
        sf::FloatRect bounds = sprite.getLocalBounds();
        sprite.setOrigin(bounds.width/2, bounds.height/2);
        if (isHoverable) {
            bounds = hoverSprite.getLocalBounds();
            hoverSprite.setOrigin(bounds.width/2, bounds.height/2);
        }
    }

    void setPosition(sf::Vector2f position) {
        sprite.setPosition(position);
        if (isHoverable) {
            hoverSprite.setPosition(position);
        }
    }

    const sf::Sprite& getSprite() const {
        return (isHoverable && isHovered) ? hoverSprite : sprite;
    }
};
