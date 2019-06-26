#ifndef ENGINE_MAIN_HPP_INCLUDE
#define ENGINE_MAIN_HPP_INCLUDE

#include "engine.hpp"

inline int engine_main(int argc, char** argv) {
    Engine::init(argc, argv);
    while (!Engine::should_close()) {
        Engine::loop();
    }
    Engine::terminate();

    return 0;
}

#endif // !ENGINE_MAIN_HPP_INCLUDE
