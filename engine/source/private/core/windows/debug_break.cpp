#include <core/debug_break.hpp>

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace anton_engine {
    void debug_break() {
        DebugBreak();
    }
}
