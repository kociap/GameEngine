#include <engine.hpp>
#include <paths_internal.hpp>

namespace anton_engine {
    int engine_main(int argc, char** argv) {
        std::filesystem::path exe_directory(argv[0], std::filesystem::path::generic_format);
        paths::set_executable_name(exe_directory.filename());
        exe_directory.remove_filename();
        paths::set_executable_directory(exe_directory);

        Engine::init();
        while (!Engine::should_close()) {
            Engine::loop();
        }
        Engine::terminate();

        return 0;
    }
} // namespace anton_engine
