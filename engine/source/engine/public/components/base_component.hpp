#ifndef ENGINE_COMPONENTS_BASE_COMPONENT_HPP_INCLUDE
#define ENGINE_COMPONENTS_BASE_COMPONENT_HPP_INCLUDE

#include "entity.hpp"
#include "object.hpp"
#include <cstdint>

class Base_Component : public Object {
public:
    Base_Component(Entity const&);
    Base_Component(Base_Component const&) = delete;
    Base_Component(Base_Component&&) noexcept = default;
    Base_Component& operator=(Base_Component const&) = delete;
    Base_Component& operator=(Base_Component&&) noexcept = default;
    virtual ~Base_Component();

    Entity const& get_entity() const;

private:
    Entity _entity;
};

#endif // !ENGINE_COMPONENTS_BASE_COMPONENT_HPP_INCLUDE