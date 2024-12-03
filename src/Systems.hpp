#pragma once
#include "Components.hpp"
#include "Registry.hpp"
#include "SparseArray.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include "server/Mediator.hpp"



class Systems
{
    public:

    static void player_control_system(registry &r, int playerId, int x, int y);



    //Fonctions codées dans le bootstrap

    static void position_system(registry &r);
    static void draw_system(registry &r, sf::RenderWindow &window);
    static void logging_system(registry &r);
};