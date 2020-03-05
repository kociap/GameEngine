#ifndef CORE_INTEGER_SEQUENCE_GENERATOR_HPP_INCLUDE
#define CORE_INTEGER_SEQUENCE_GENERATOR_HPP_INCLUDE

#include <cstdint>

namespace anton_engine {
    class Integer_Sequence_Generator {
    public:
        Integer_Sequence_Generator(uint64_t start_number = 0): _next(start_number) {}

        uint64_t next() {
            return _next++;
        }

    private:
        uint64_t _next;
    };
} // namespace anton_engine

#endif // !CORE_INTEGER_SEQUENCE_GENERATOR_HPP_INCLUDE
