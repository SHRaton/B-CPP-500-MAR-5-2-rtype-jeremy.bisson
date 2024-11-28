#pragma once
#include <cstddef>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <unordered_map>
#include "Utils.hpp"


class Sprite {
    public:
        Sprite() = default;
        Sprite(const std::string& texturePath, bool scrolling = false, float speed = 1.0f,
                int interval = 50, const std::string& hoverTexturePath = "");

        void update();
        void setHovered(bool hover);
        bool isMouseOver(const sf::RenderWindow& window);
        void setScale(sf::Vector2f scale);
        void setOriginToMiddle();
        void setPosition(sf::Vector2f position);
        sf::Sprite getSprite();

    private:
        sf::Sprite sprite;
        sf::Sprite hoverSprite;
        bool isScrolling = false;
        sf::Clock clock;
        float scrollSpeed = 0;
        int updateInterval = 0;
        bool isHoverable = false;
        bool isHovered = false;
};
