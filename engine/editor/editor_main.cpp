#include <editor.hpp>
#include <paths.hpp>

namespace anton_engine {
    int editor_main() {
        Editor::init();
        while (!Editor::should_close()) {
            Editor::loop();
        }
        Editor::terminate();
        return 0;
    }
} // namespace anton_engine
