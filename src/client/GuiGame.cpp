#include "Core.hpp"

void Core::gui_game() {
    sprites_game["blueGalaxy"].setScale({2, 2});

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed || 
                (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)) {
                window.close();
            }

            if (event.type == sf::Event::MouseMoved) {
                sprites_menu["play"].setHovered(sprites_menu["play"].isMouseOver(window));
                sprites_menu["quit"].setHovered(sprites_menu["quit"].isMouseOver(window));
            }
            if (event.type == sf::Event::KeyPressed) {
                network->send(GameAction::MOVE);
            }

            if (event.type == sf::Event::MouseButtonPressed && 
                event.mouseButton.button == sf::Mouse::Left) {
                if (sprites_menu["play"].isMouseOver(window)) {
                    gui_login();
                }
                if (sprites_menu["quit"].isMouseOver(window)) {
                    window.close();
                }
            }
        }

        for (auto& [name, sprite] : sprites_game) {
            if (name == "blueGalaxy") {
                sprite.update_xy(1920, 0);
            } else {
                sprite.update();
            }
        }
        window.clear();
        for (const auto& name : drawOrder_game) {
            window.draw(sprites_game[name].getSprite());
        }
        window.display();
    }
}
