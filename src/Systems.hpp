#include "Components.hpp"
#include "Registry.hpp"
#include "SparseArray.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>

#pragma once

void control_system(registry &r, char c);
void position_system(registry &r);
void draw_system(registry &r, sf::RenderWindow &window);
void logging_system(registry &r);