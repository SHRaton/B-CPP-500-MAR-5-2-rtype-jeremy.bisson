#pragma once
#include <iostream>
#include <cstddef> // For size_t

/**
 * @brief Classe représentant une entité dans le système ECS
 * @class Entity
 * 
 * @details Cette classe encapsule un identifiant unique pour chaque entité
 * du jeu et fournit une conversion implicite vers size_t.
 * 
 * @startuml{Entity_class.png}
 * class Entity {
 * - size_t id
 * + Entity(size_t value)
 * + operator size_t()
 * }
 * @enduml
 * 
 * @startuml{Entity_Relations.png}
 * Entity "1" -- "*" component.health : peut avoir
 * Entity "1" -- "*" component.position : peut avoir
 * Entity "1" -- "*" component.velocity : peut avoir
 * Entity "1" -- "*" component.drawable : peut avoir
 * @enduml
 */

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

