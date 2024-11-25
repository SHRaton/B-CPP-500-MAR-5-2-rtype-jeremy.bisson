#include "Entity.hpp"
#include "Registry.hpp"
#include "SparseArray.hpp"
#include "Components.hpp"
#include "Systems.hpp"
#include <cstddef>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

int main(int argc, char **argv)
{
    sf::RenderWindow window(sf::VideoMode(1920, 1200), "ECS Test");

    registry reg;
    reg.register_component<component::position>();
    reg.register_component<component::velocity>();
    reg.register_component<component::drawable>();
    reg.register_component<component::controllable>();

    // Création du joueur
    Entity player = reg.spawn_entity();
    reg.emplace_component<component::position>(player, component::position{0, 0});  // Initialisation correcte
    reg.emplace_component<component::velocity>(player, component::velocity{0, 0}); // Initialisation correcte
    reg.emplace_component<component::drawable>(player, component::drawable{"../player.png"});
    reg.emplace_component<component::controllable>(player, component::controllable{true});

    // Création de l'ennemi
    Entity enemy = reg.spawn_entity();
    reg.emplace_component<component::position>(enemy, component::position{100, 100}); // Initialisation correcte
    reg.emplace_component<component::velocity>(enemy, component::velocity{1, 1});    // Initialisation correcte
    reg.emplace_component<component::drawable>(enemy, component::drawable{"../enemy.png"});
    reg.emplace_component<component::controllable>(enemy, component::controllable{false});

    while(window.isOpen()){
        sf::Event event;
        while(window.pollEvent(event)){
            if(event.type == sf::Event::Closed){
                window.close();
            }
        }
        window.clear();
        draw_system(reg, window);
        window.display();
    }
    return 0;
}
