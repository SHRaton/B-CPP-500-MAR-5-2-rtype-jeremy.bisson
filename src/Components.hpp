#include <string>

#pragma once

namespace component{
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