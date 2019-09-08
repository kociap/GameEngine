#include <editor.hpp>
#include <paths.hpp>

int editor_main() {
    Editor::init();
    while (!Editor::should_close()) {
        Editor::loop();
    }
    Editor::terminate();
    return 0;
}
