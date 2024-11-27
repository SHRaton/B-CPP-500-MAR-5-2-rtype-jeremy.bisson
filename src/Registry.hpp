#pragma once
#include <unordered_map>
#include <any>
#include <functional>
#include <typeindex>
#include <stdexcept>
#include "SparseArray.hpp"
#include "Entity.hpp"


class registry {
public:
    template <class Component>
    sparse_array<Component>& register_component() {
        std::type_index type = typeid(Component);

        // Check if the component type is already registered
        if (_components_arrays.find(type) != _components_arrays.end()) {
            throw std::runtime_error("Component type already registered.");
        }

        // Insert a new sparse_array for the component type
        _components_arrays[type] = sparse_array<Component>();

        _erase_functions[type] = [](registry& reg, Entity const& entity) {
            auto& components = reg.get_components<Component>();
            components.erase(entity); // Remove the component for the specified entity
        };

        return std::any_cast<sparse_array<Component>&>(_components_arrays[type]);
    }

    template <class Component>
    sparse_array<Component>& get_components() {
        std::type_index type = typeid(Component);

        // Find the component array in the container
        auto it = _components_arrays.find(type);
        if (it == _components_arrays.end()) {
            throw std::runtime_error("Component type not registered.");
        }

        // Cast the type-erased array back to sparse_array<Component>
        return std::any_cast<sparse_array<Component>&>(it->second);
    }

    template <class Component>
    const sparse_array<Component>& get_components() const {
        std::type_index type = typeid(Component);

        // Find the component array in the container
        auto it = _components_arrays.find(type);
        if (it == _components_arrays.end()) {
            throw std::runtime_error("Component type not registered.");
        }

        // Cast the type-erased array back to sparse_array<Component>
        return std::any_cast<const sparse_array<Component>&>(it->second);
    }

//==================================ENTITY MANGAGING

    Entity spawn_entity()
    {
        size_t new_entity_index = _entities.size();
        Entity new_entity(new_entity_index);
        _entities.push_back(new_entity);
        return new_entity;
    }

    Entity entity_from_index(std::size_t idx)
    {
        if (idx >= _entities.size())
        {
            throw std::out_of_range("Entity index out of range");
        }
        return _entities[idx];
    }

    void kill_entity(Entity const &e)
    {
        for (auto& [_, erase_function] : _erase_functions)
        {
            erase_function(*this, e);
        }
    }

    template <typename Component>
    typename sparse_array<Component>::reference_type add_component(Entity const &to,
    Component&& c)
    {
        auto& components = get_components<Component>();
        return components.insert_at(to, std::forward<Component>(c));
    }

    template<typename Component, typename... Params>
    typename sparse_array<Component>::reference_type emplace_component(Entity const &to,
    Params&&... p)
    {
        auto &components = get_components<Component>();
        return components.emplace_at(to, std::forward<Params>(p)...);
    }


    template<typename Component>
    void remove_component(Entity const &from)
    {
        auto &components = get_components<Component>();
        components.erase(from);
    }





//==================================================






private:
    std::unordered_map<std::type_index, std::any> _components_arrays;
    std::unordered_map<std::type_index, std::function<void(registry&, Entity const&)>> _erase_functions;
    std::vector<Entity> _entities;
};
