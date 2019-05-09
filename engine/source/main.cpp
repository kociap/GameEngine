#include "build_config.hpp"

#if !GE_WITH_EDITOR
#include "engine.hpp"

int main(int argc, char** argv) {
    Engine::init(argc, argv);
    while (!Engine::should_close()) {
        Engine::loop();
    }
    Engine::terminate();

    return 0;
}

#endif // GE_WITH_EDITOR
