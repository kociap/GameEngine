#include "object.hpp"

bool operator==(Object const& obj1, Object const& obj2) {
    return obj1.id == obj2.id;
}
