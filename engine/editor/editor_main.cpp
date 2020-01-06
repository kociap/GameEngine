#include <editor.hpp>
#include <paths.hpp>
#include <paths_internal.hpp>

namespace anton_engine {
    int editor_main(int argc, char** argv) {
        // Required arguments: path to executable and path to the project file
        // if (argc < 2) {
        //     throw std::runtime_error("Missing path to project file");
        // }

        // if(!std::filesystem::exists(argv[1])) {
        //     throw std::runtime_error("Specified project file does not exist");
        // }

        std::filesystem::path exe_directory(argv[0], std::filesystem::path::generic_format);
        paths::set_engine_executable_name(exe_directory.filename());
        exe_directory.remove_filename();
        paths::set_engine_executable_directory(exe_directory);
        //std::filesystem::path project_directory(argv[1], std::filesystem::path::generic_format);
        std::filesystem::path project_directory("C:/Users/An0num0us/Documents/GameEngine_Game/GameEngine_Game.geproject",
                                                std::filesystem::path::generic_format);
        project_directory.remove_filename();
        paths::set_project_directory(project_directory);

        // TODO Validate project file

        Editor::init();
        while (!Editor::should_close()) {
            Editor::loop();
        }
        Editor::terminate();
        return 0;
    }
} // namespace anton_engine
