#pragma once
#include <iostream>
#include <cstddef> // For size_t


class Entity {
private:
    size_t id;

public:
    // Explicit constructor: prevents implicit conversion from size_t to Entity
    explicit Entity(size_t value) : id(value) {}

    // Implicit conversion operator: allows implicit conversion to size_t
    operator size_t() const {
        return id;
    }
};

