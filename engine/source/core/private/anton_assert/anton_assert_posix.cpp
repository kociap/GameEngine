#include <anton_assert.hpp>

#include <stdio.h>
#include <stdlib.h>

namespace anton_engine {
    void anton_assert(char const* message, char const* file, unsigned long long line) {
        fprintf(stderr, "Assertion failed:\n%s\nIn file %s on line %llu", message, file, line);
        fflush(stderr);
        abort();
    }
} // namespace anton_engine