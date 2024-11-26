#include "Menu_Login.hpp"
#include "Utils.hpp"

void menu_login(sf::RenderWindow& window) {
    std::map<std::string, Sprite> sprites = {
        {"background", Sprite("../ressources/background/background.png", true)},
        {"small_stars", Sprite("../ressources/background/small_stars.png", true, 1.0f, 20)},
        {"poudreBleu", Sprite("../ressources/background/poudreBleu.png", true, 1.0f, 90)},
        {"rtype", Sprite("../ressources/background/rtype.png", false)},
        {"box_opacity", Sprite("../ressources/background/box_opacity2.png", false)}
    };
    std::vector<std::string> drawOrder = {
        "background",
        "small_stars",
        "poudreBleu",
        "rtype",
        "box_opacity"
    };

    sprites["poudreBleu"].setScale({2, 2});
    sprites["rtype"].setOriginToMiddle();
    sprites["rtype"].setScale({1.4, 1.4});
    sprites["rtype"].setPosition({960 , 100});
    sprites["box_opacity"].setOriginToMiddle();
    sprites["box_opacity"].setScale({1.5, 1.5});
    sprites["box_opacity"].setPosition({960 , 600});

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
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