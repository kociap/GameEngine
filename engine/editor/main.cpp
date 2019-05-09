#include "build_config.hpp"

#include "editor.hpp"
#include "engine.hpp"

#if GE_WITH_EDITOR
int main(int argc, char** argv) {
    Engine::init(argc, argv);
    Editor::init();
    while (!Editor::should_close()) {
        Editor::loop();
    }
    Editor::terminate();
    Engine::terminate();

    return 0;
}

#endif // GE_WITH_EDITOR