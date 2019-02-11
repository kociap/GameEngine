#ifndef ENGINE_OBJECT_HPP_INCLUDE
#define ENGINE_OBJECT_HPP_INCLUDE

#include "guid.hpp"

class Object {
public:
    Object() = default;
    Object(Object const&) = delete;
    Object(Object&&) noexcept = default;
    Object& operator=(Object const&) = delete;
    Object& operator=(Object&&) noexcept = default;
    ~Object() = default;

    Guid id;
};

bool operator==(Object const&, Object const&);

#endif // !ENGINE_OBJECT_HPP_INCLUDE