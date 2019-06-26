#ifndef CORE_INTEGER_SEQUENCE_GENERATOR_HPP_INCLUDE
#define CORE_INTEGER_SEQUENCE_GENERATOR_HPP_INCLUDE

#include "config.hpp"

class Integer_Sequence_Generator {
public:
    Integer_Sequence_Generator(id_type start_number = 0): _next(start_number) {}

    id_type next() {
        return _next++;
    }

private:
    id_type _next;
};

#endif // !CORE_INTEGER_SEQUENCE_GENERATOR_HPP_INCLUDE
