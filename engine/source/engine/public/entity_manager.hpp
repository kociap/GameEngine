#ifndef ENGINE_ENTITY_MANAGER_HPP_INCLUDE
#define ENGINE_ENTITY_MANAGER_HPP_INCLUDE

#include "containers/swapping_pool.hpp"
#include "entity.hpp"

class Entity_Manager {
public:
    using iterator = Swapping_Pool<Entity>::iterator;

	template<typename... Args>
    Entity emplace(Args&&... args) {
        return entities.add(std::forward<Args>(args)...);
	}

    iterator begin();
    iterator end();

private:
    Swapping_Pool<Entity> entities;
};

#endif // !ENGINE_ENTITY_MANAGER_HPP_INCLUDE