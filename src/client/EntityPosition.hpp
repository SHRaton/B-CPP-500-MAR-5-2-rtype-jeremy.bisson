#pragma once

class EntityPosition {
public:
    EntityPosition(int type, int x, int y) : type(type), x(x), y(y) {}
    int type;
    int x;
    int y;
};