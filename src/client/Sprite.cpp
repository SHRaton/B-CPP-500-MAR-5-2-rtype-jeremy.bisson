/*
** EPITECH PROJECT, 2024
** B-CPP-500-MAR-5-2-rtype-jeremy.bisson
** File description:
** Sprite
*/

#include "Sprite.hpp"

Sprite::Sprite(const std::string& texturePath, bool scrolling, float speed, int interval, const std::string& hoverTexturePath)
{
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

void Sprite::update_xy(int x, int y)
{
    if (isScrolling) {
        sf::Time elapsed = clock.restart();
        float deltaTime = elapsed.asSeconds();
        sprite.move(-scrollSpeed * 60 * deltaTime, 0);
        if (sprite.getPosition().x <= -sprite.getGlobalBounds().width / 2) {
            sprite.setPosition(x, y);
        }
    }
}

void Sprite::update()
{
    if (isScrolling) {
        sf::Time elapsed = clock.restart();
        float deltaTime = elapsed.asSeconds();
        sprite.move(-scrollSpeed * 60 * deltaTime, 0);

        if (!isGameBackground) {
            if (sprite.getPosition().x <= -sprite.getGlobalBounds().width / 2) {
                sprite.setPosition(0, 0);
            }
        }
    }
}

void Sprite::setHovered(bool hover)
{
    isHovered = hover;
}

bool Sprite::isMouseOver(const sf::RenderWindow& window)
{
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    return sprite.getGlobalBounds().contains(mousePos.x, mousePos.y);
}

void Sprite::setScale(sf::Vector2f scale)
{
    sprite.setScale(scale);
    if (isHoverable) {
        hoverSprite.setScale(scale);
    }
}

void Sprite::setOriginToMiddle()
{
    sf::FloatRect bounds = sprite.getLocalBounds();
    sprite.setOrigin(bounds.width/2, bounds.height/2);
    if (isHoverable) {
        bounds = hoverSprite.getLocalBounds();
        hoverSprite.setOrigin(bounds.width/2, bounds.height/2);
    }
}

void Sprite::setPosition(sf::Vector2f position)
{
    sprite.setPosition(position);
    if (isHoverable) {
        hoverSprite.setPosition(position);
    }
}

sf::Sprite Sprite::getSprite()
{
    return (isHoverable && isHovered) ? hoverSprite : sprite;
}
