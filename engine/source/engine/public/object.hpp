#ifndef ENGINE_OBJECT_HPP_INCLUDE
#define ENGINE_OBJECT_HPP_INCLUDE

#include "guid.hpp"

class Object {
public:
    Guid id;
};

bool operator==(Object const&, Object const&);

#endif // !ENGINE_OBJECT_HPP_INCLUDE