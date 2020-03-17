#include <core/debug_break.hpp>

#include <signal.h>

namespace anton_engine {
    void debug_break() {
        raise(SIGTRAP);
    }
}
