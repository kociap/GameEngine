#ifndef ENGINE_ENTITY_MANAGER_HPP_INCLUDE
#define ENGINE_ENTITY_MANAGER_HPP_INCLUDE

#include "containers/vector.hpp"
#include "entity.hpp"

class Entity_Manager {
public:
    using iterator = containers::Vector<Entity>::iterator;

    template <typename... Args>
    Entity emplace(Args&&... args) {
        return entities.emplace_back(std::forward<Args>(args)...);
    }

    iterator begin();
    iterator end();

private:
    containers::Vector<Entity> entities;
};

#endif // !ENGINE_ENTITY_MANAGER_HPP_INCLUDE