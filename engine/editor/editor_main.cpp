#include <editor.hpp>
#include <paths.hpp>

int editor_main() {
    std::filesystem::path exe_dir = paths::engine_executable_directory();
    char* exe_ptr = const_cast<char*>(reinterpret_cast<char const*>(exe_dir.c_str()));
    Editor::init(1, &exe_ptr);
    while (!Editor::should_close()) {
        Editor::loop();
    }
    Editor::terminate();
    return 0;
}
