#pragma once

#include <string>


namespace component{
    typedef struct health {
        int hp;
    } Health;

    typedef struct damage {
        int dmg;
    } Damage;

    typedef struct name {
        std::string name; //pour savoir quel sprite afficher ex: Enemy1 = spriteEnemy1.png
    } Name;

    typedef struct position {
        int x;
        int y;
    } Position;

    typedef struct velocity {
        int vx;
        int vy;
    } Velocity;

    typedef struct drawable {
        std::string file;
    } Drawable;

    typedef struct controllable {
        bool is_controllable;
    } Controllable;
}