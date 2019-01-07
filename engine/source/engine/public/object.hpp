#ifndef ENGINE_OBJECT_HPP_INCLUDE
#define ENGINE_OBJECT_HPP_INCLUDE

#include "components/base_component.hpp"
#include <cstdint>
#include <map>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <typeindex>
#include <vector>

class Object {
protected:
    uint64_t id;
};

#endif // !ENGINE_OBJECT_HPP_INCLUDE