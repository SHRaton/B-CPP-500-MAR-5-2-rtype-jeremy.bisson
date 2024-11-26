#include "Menu.hpp"
#include "Menu_Login.hpp"
#include "Utils.hpp"


// Exemple de la fonction menu
void menu(sf::RenderWindow& window) {
    std::map<std::string, Sprite> sprites = {
        {"background", Sprite("./ressources/background/background.png", true)},
        {"small_stars", Sprite("./ressources/background/small_stars.png", true, 1.0f, 20)},
        {"poudreBleu", Sprite("./ressources/background/poudreBleu.png", true, 1.0f, 90)},
        {"rtype", Sprite("./ressources/background/rtype.png", false)},
        {"play", Sprite("./ressources/background/play.png", false, 1.0f, 50, "./ressources/background/play_hover.png")},
        {"quit", Sprite("./ressources/background/quit.png", false, 1.0f, 50, "./ressources/background/quit_hover2.png")}
    };
    std::vector<std::string> drawOrder = {
        "background",
        "small_stars",
        "poudreBleu",
        "rtype",
        "play",
        "quit"
    };

    sprites["poudreBleu"].setScale({2, 2});
    sprites["rtype"].setOriginToMiddle();
    sprites["rtype"].setScale({1.4, 1.4});
    sprites["rtype"].setPosition({960, 100});
    sprites["play"].setOriginToMiddle();
    sprites["play"].setScale({0.5, 0.5});
    sprites["play"].setPosition({960, 500});
    sprites["quit"].setOriginToMiddle();
    sprites["quit"].setScale({0.5, 0.5});
    sprites["quit"].setPosition({960, 800});

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed || 
                (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)) {
                window.close();
            }

            if (event.type == sf::Event::MouseMoved) {
                sprites["play"].setHovered(sprites["play"].isMouseOver(window));
                sprites["quit"].setHovered(sprites["quit"].isMouseOver(window));
            }

            if (event.type == sf::Event::MouseButtonPressed && 
                event.mouseButton.button == sf::Mouse::Left) {
                if (sprites["play"].isMouseOver(window)) {
                    menu_login(window);
                }
                if (sprites["quit"].isMouseOver(window)) {
                    window.close();
                }
            }
        }

        for (auto& [name, sprite] : sprites) {
            sprite.update();
        }

        window.clear();
        for (const auto& name : drawOrder) {
            window.draw(sprites[name].getSprite());
        }
        window.display();
    }
}