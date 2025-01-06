#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <string>
#include <vector>
#include <chrono>

/**
 * @brief Namespace contenant tous les composants du jeu
 * @namespace component
 * 
 * @startuml{Components_diagram.png}
 * namespace component {
 *   struct health {
 *     + int hp
 *   }
 *   
 *   struct type {
 *     + int type
 *   }
 *   
 *   struct damage {
 *     + int dmg
 *   }
 *   
 *   struct position {
 *     + int x
 *     + int y
 *   }
 *   
 *   struct velocity {
 *     + int vx
 *     + int vy
 *   }
 *   
 *   struct drawable {
 *     + sf::Sprite sprite
 *   }
 *   
 *   struct controllable {
 *     + bool is_controllable
 *   }
 *   
 *   struct invincible {
 *     + bool is_invincible
 *     + time_point expiration_time
 *   }
 *   
 *   struct size {
 *     + int width
 *     + int height
 *   }
 *   
 *   struct triple_shot {
 *     + bool is_active
 *     + time_point activation_time
 *   }
 *   
 *   struct score {
 *     + int value
 *   }
 * }
 * @enduml
 */

namespace component{
    typedef struct health {
        int hp;
    } Health;

    typedef struct type {
        int type;
    } Type;

    typedef struct damage {
        int dmg;
    } Damage;

    typedef struct position {
        int x;
        int y;
    } Position;

    typedef struct velocity {
        int vx;
        int vy;
    } Velocity;

    typedef struct drawable {
        sf::Sprite sprite;
    } Drawable;

    typedef struct controllable {
        bool is_controllable;
    } Controllable;

    typedef struct invincible {
        bool is_invincible;
        std::chrono::steady_clock::time_point expiration_time;
    } Invincible;

    typedef struct size {
        int width;
        int height;
    } Size;

    struct triple_shot {
        bool is_active = false;
        std::chrono::steady_clock::time_point activation_time;
    };

    struct force {
        bool is_active = false;
        int is_front = 0;
        std::chrono::steady_clock::time_point activation_time;
    };

    struct score {
        int value;
    };
}