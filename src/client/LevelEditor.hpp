#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include "EntityPosition.hpp"
#include "CurrentMap.hpp"
#include <iostream>
#include <vector>
#include <string>

class LevelEditor {
public:
    LevelEditor(std::vector<EntityPosition> entities, CurrentMap current_map);
    ~LevelEditor();

    void run();
    void drawAll();
    void addAllExistingEntities(std::vector<EntityPosition> entities);
    void addExistingEntity(int type, int x, int y);
    void setAssets();
    void updateCurrentMap();
    int spawnEntity(sf::Event event);

private:
    Utils utils;
    sf::RenderWindow window;
    sf::Sprite background;
    int levelSelected;
    CurrentMap currentMap;
    std::vector<sf::Sprite> entitySprites;
    std::string filename;
    sf::RectangleShape progressBackground;
    sf::RectangleShape progressBar;
};