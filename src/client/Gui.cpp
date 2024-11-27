#include "Gui.hpp"

void Core::gui(int argc, char **argv)
{
    send_network_message("11111");
    gui_menu(window);

    // Gameplay du jeu
    while(window.isOpen()) {
        sf::Event event;
        while(window.pollEvent(event)){
            if(event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::KeyPressed) {
                control_system(reg, event.key.code);
            }
        }
        position_system(reg);
        window.clear();
        draw_system(reg, window);
        window.display();
    }
}

void Core::loadAssets()
{
    reg.register_component<component::position>();
    reg.register_component<component::velocity>();
    reg.register_component<component::drawable>();
    reg.register_component<component::controllable>();

    // Création du joueur
    Entity player = reg.spawn_entity();
    reg.emplace_component<component::position>(player, component::position{0, 0});
    reg.emplace_component<component::velocity>(player, component::velocity{0, 0});
    reg.emplace_component<component::drawable>(player, component::drawable{"./ressources/sprites/jinx.png"});
    reg.emplace_component<component::controllable>(player, component::controllable{true});

    // Création de l'ennemi
    Entity enemy = reg.spawn_entity();
    reg.emplace_component<component::position>(enemy, component::position{100, 100});
    reg.emplace_component<component::velocity>(enemy, component::velocity{1, 1});
    reg.emplace_component<component::drawable>(enemy, component::drawable{"./ressources/sprites/jinx.png"});
    reg.emplace_component<component::controllable>(enemy, component::controllable{false});


    sprites_menu = {
        {"background", Sprite("../ressources/background/background.png", true)},
        {"small_stars", Sprite("../ressources/background/small_stars.png", true, 1.0f, 20)},
        {"poudreBleu", Sprite("../ressources/background/poudreBleu.png", true, 1.0f, 90)},
        {"rtype", Sprite("../ressources/background/rtype.png", false)},
        {"play", Sprite("../ressources/background/play.png", false, 1.0f, 50, "../ressources/background/play_hover.png")},
        {"quit", Sprite("../ressources/background/quit.png", false, 1.0f, 50, "../ressources/background/quit_hover2.png")}
    };
    drawOrder_menu = {
        "background",
        "small_stars",
        "poudreBleu",
        "rtype",
        "play",
        "quit"
    };

    sprites_login = {
        {"background", Sprite("../ressources/background/background.png", true)},
        {"small_stars", Sprite("../ressources/background/small_stars.png", true, 1.0f, 20)},
        {"poudreBleu", Sprite("../ressources/background/poudreBleu.png", true, 1.0f, 90)},
        {"rtype", Sprite("../ressources/background/rtype.png", false)},
        {"box_opacity", Sprite("../ressources/background/box_opacity2.png", false)}
    };
    drawOrder_login = {
        "background",
        "small_stars",
        "poudreBleu",
        "rtype",
        "box_opacity"
    };
}