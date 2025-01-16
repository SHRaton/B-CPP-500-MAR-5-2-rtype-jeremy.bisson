#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include "Utils.hpp"

class KeyBinding {
    private:
        sf::Keyboard::Key key;
        std::string actionName;
        sf::Text displayText;
        bool isWaitingForKey;

    public:
        KeyBinding(sf::Keyboard::Key defaultKey = sf::Keyboard::Unknown, 
                  const std::string& name = "");
        void initializeText(const sf::Font& font, float x, float y);
        void setKey(sf::Keyboard::Key newKey);
        void setWaitingForKey(bool waiting);
        void updateDisplayText();
        sf::Keyboard::Key getKey() const;
        const std::string& getActionName() const;
        const sf::Text& getDisplayText() const;
        bool isWaiting() const;
        static std::string getKeyName(sf::Keyboard::Key key);
};
