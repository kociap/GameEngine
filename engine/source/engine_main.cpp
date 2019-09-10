#include <engine.hpp>

namespace anton_engine {
    int engine_main() {
        Engine::init();
        while (!Engine::should_close()) {
            Engine::loop();
        }
        Engine::terminate();

        return 0;
    }
} // namespace anton_engine
