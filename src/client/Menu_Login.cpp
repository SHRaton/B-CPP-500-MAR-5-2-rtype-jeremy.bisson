#include "Gui.hpp"

void Core::gui_login(sf::RenderWindow& window) {
    sprites_login["poudreBleu"].setScale({2, 2});
    sprites_login["rtype"].setOriginToMiddle();
    sprites_login["rtype"].setScale({1.4, 1.4});
    sprites_login["rtype"].setPosition({960 , 100});
    sprites_login["box_opacity"].setOriginToMiddle();
    sprites_login["box_opacity"].setScale({1.5, 1.5});
    sprites_login["box_opacity"].setPosition({960 , 600});

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }
        for (auto& [name, sprite] : sprites_login) {
            sprite.update();
        }
        window.clear();
        for (const auto& name : drawOrder_login) {
            window.draw(sprites_login[name].getSprite());
        }
        window.display();
    }
}