#pragma once
#include "Components.hpp"
#include "Registry.hpp"
#include "SparseArray.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include "server/Mediator.hpp"

/**
 * @brief Système principal de gestion des composants
 * @class Systems
 * 
 * @details Cette classe contient tous les systèmes qui manipulent les composants
 * du jeu R-Type, notamment le contrôle des joueurs, les positions et l'affichage.
 * 
 * @startuml{Systems_class.png}
 * class Systems {
 * + {static} player_control_system(registry &r, int playerId, int x, int y)
 * + {static} position_system(registry &r)
 * + {static} draw_system(registry &r, sf::RenderTexture &renderTexture)
 * + {static} logging_system(registry &r)
 * }
 * @enduml
 * 
 * @see component
 * @see registry
 */

class Systems
{
    public:

    static void player_control_system(registry &r, int playerId, int x, int y);



    //Fonctions codées dans le bootstrap

    static void position_system(registry &r);
    static void draw_system(registry &r, sf::RenderTexture &renderTexture);
    static void logging_system(registry &r);
};