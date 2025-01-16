#include "KeyBinding.hpp"

// Dans KeyBinding.cpp
KeyBinding::KeyBinding(sf::Keyboard::Key defaultKey, const std::string& name)
    : key(defaultKey)
    , actionName(name)
    , isWaitingForKey(false)
{
}

void KeyBinding::initializeText(const sf::Font& font, float x, float y) {
    displayText.setFont(font);
    displayText.setCharacterSize(30);
    displayText.setFillColor(sf::Color::White);
    displayText.setPosition(x, y);
    updateDisplayText();
}

void KeyBinding::setKey(sf::Keyboard::Key newKey) {
    key = newKey;
    updateDisplayText();
}

void KeyBinding::setWaitingForKey(bool waiting) {
    isWaitingForKey = waiting;
    updateDisplayText();
}

void KeyBinding::updateDisplayText() {
    std::string keyName = isWaitingForKey ? "Press any key..." : getKeyName(key);
    displayText.setString(actionName + ": " + keyName);
}

sf::Keyboard::Key KeyBinding::getKey() const {
    return key;
}

const std::string& KeyBinding::getActionName() const {
    return actionName;
}

const sf::Text& KeyBinding::getDisplayText() const {
    return displayText;
}

bool KeyBinding::isWaiting() const {
    return isWaitingForKey;
}

std::string KeyBinding::getKeyName(sf::Keyboard::Key key) {
    Utils utils;
    return utils.keyToString(key, false);
}