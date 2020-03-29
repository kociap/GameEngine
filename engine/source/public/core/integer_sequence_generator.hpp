#ifndef CORE_INTEGER_SEQUENCE_GENERATOR_HPP_INCLUDE
#define CORE_INTEGER_SEQUENCE_GENERATOR_HPP_INCLUDE

#include <core/types.hpp>

namespace anton_engine {
    class Integer_Sequence_Generator {
    public:
        Integer_Sequence_Generator(u64 start_number = 0): _next(start_number) {}

        u64 next() {
            return _next++;
        }

    private:
        u64 _next;
    };
} // namespace anton_engine

#endif // !CORE_INTEGER_SEQUENCE_GENERATOR_HPP_INCLUDE
