#pragma once

#include <cstddef>
#include "Utils.hpp"
#include <sstream>


class CurrentMap {
    public:
        CurrentMap(std::string filepath, std::string map, std::string mob1, int mob1_frames, std::string mob2, int mob2_frames, std::string boss, int boss_frames, std::string obstacle) :
            filepath(filepath),
            map(map),
            mob1(mob1),
            mob1_frames(mob1_frames),
            mob2(mob2),
            mob2_frames(mob2_frames),
            boss(boss),
            boss_frames(boss_frames),
            obstacle(obstacle)
        {
            Utils utils;
            map_sprite = utils.cat("../ressources/sprites/" + map);
            mob1_sprite = utils.cat("../ressources/sprites/" + mob1);
            mob2_sprite = utils.cat("../ressources/sprites/" + mob2);
            boss_sprite = utils.cat("../ressources/sprites/" + boss);
            obstacle_sprite = utils.cat("../ressources/sprites/" + obstacle);

        };
        ~CurrentMap() = default;

        sf::Sprite &getBackgroundSprite() { return map_sprite; }
        sf::Sprite &getMob1Sprite() { return mob1_sprite; }
        sf::Sprite &getMob2Sprite() { return mob2_sprite; }
        sf::Sprite &getBossSprite() { return boss_sprite; }
        sf::Sprite &getObstacleSprite() { return obstacle_sprite; }

        std::string getFilepath() { return filepath; }
        std::string getMap() { return map; }
        std::string getMob1() { return mob1; }
        std::string getMob2() { return mob2; }
        std::string getBoss() { return boss; }
        std::string getObstacle() { return obstacle; }

        int getMob1Frames() { return mob1_frames; }
        int getMob2Frames() { return mob2_frames; }
        int getBossFrames() { return boss_frames; }


    private:
        std::string filepath;
        std::string map;
        std::string mob1;
        int mob1_frames;
        std::string mob2;
        int mob2_frames;
        std::string boss;
        int boss_frames;
        std::string obstacle;

        sf::Sprite map_sprite;
        sf::Sprite mob1_sprite;
        sf::Sprite mob2_sprite;
        sf::Sprite boss_sprite;
        sf::Sprite obstacle_sprite;
};