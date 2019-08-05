#include <engine.hpp>

int engine_main() {
    Engine::init();
    while (!Engine::should_close()) {
        Engine::loop();
    }
    Engine::terminate();

    return 0;
}
