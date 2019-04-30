#ifndef CORE_ID_GENERATOR_HPP_INCLUDE
#define CORE_ID_GENERATOR_HPP_INCLUDE

#include "config.hpp"

template <typename...>
class ID_Generator {
public:
    static id_type next() {
        static id_type _next = 0;
        return _next++;
    }
};

#endif // !CORE_ID_GENERATOR_HPP_INCLUDE
