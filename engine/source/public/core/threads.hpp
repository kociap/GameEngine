#ifndef CORE_THREADS_HPP_INCLUDE
#define CORE_THREADS_HPP_INCLUDE

#include <core/types.hpp>

namespace anton_engine::threads {
    struct Timespec {
        i64 seconds;
        i64 nanoseconds;
    };

    void sleep(Timespec const& duration);
}

#endif // !CORE_THREADS_HPP_INCLUDE
