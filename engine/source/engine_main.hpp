#ifndef ENGINE_MAIN_HPP_INCLUDE
#define ENGINE_MAIN_HPP_INCLUDE

#include "engine.hpp"

inline int engine_main() {
    Engine::init();
    while (!Engine::should_close()) {
        Engine::loop();
    }
    Engine::terminate();

    return 0;
}

#endif // !ENGINE_MAIN_HPP_INCLUDE
