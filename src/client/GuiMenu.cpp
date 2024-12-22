#include "Core.hpp"

// Exemple de la fonction menu
void Core::gui_menu() {

    sprites_menu["poudreBleu"].setScale({2, 2});
    sprites_menu["rtype"].setOriginToMiddle();
    sprites_menu["rtype"].setScale({1.4, 1.4});
    sprites_menu["rtype"].setPosition({960, 100});
    sprites_menu["play"].setOriginToMiddle();
    sprites_menu["play"].setScale({0.5, 0.5});
    sprites_menu["play"].setPosition({960, 500});
    sprites_menu["quit"].setOriginToMiddle();
    sprites_menu["quit"].setScale({0.5, 0.5});
    sprites_menu["quit"].setPosition({960, 800});

    sprites_menu["option"].setOriginToMiddle();
    sprites_menu["option"].setScale({0.2, 0.2});
    sprites_menu["option"].setPosition({1850, 950});

    menuMusic.play();


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
                sprites_menu["option"].setHovered(sprites_menu["option"].isMouseOver(window));
            }

            if (event.type == sf::Event::MouseButtonPressed && 
                event.mouseButton.button == sf::Mouse::Left) {
                if (sprites_menu["play"].isMouseOver(window)) {
                    buttonSound_click.play();
                    gui_login();
                }
                if (sprites_menu["quit"].isMouseOver(window)) {
                    buttonSound_click.play();
                    window.close();
                }
                if (sprites_menu["option"].isMouseOver(window)) {
                    buttonSound_click.play();
                    gui_option();
                }
            }
        }

        for (auto& [name, sprite] : sprites_menu) {
            if (name == "blueGalaxy") {
                sprite.update_xy(1920, 0);
            } else {
                sprite.update();
            }
        }

        window.clear();
        renderTexture.clear(sf::Color::Black);
        for (const auto& name : drawOrder_menu) {
            renderTexture.draw(sprites_menu[name].getSprite());
        }
        renderTexture.display();
        sf::Sprite screenSprite(renderTexture.getTexture());
        if (daltonismType != DaltonismType::NONE) {
            window.draw(screenSprite, &daltonismShader);
        } else {
            window.draw(screenSprite);
        }
        window.display();
    }
}
