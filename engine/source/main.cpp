#include <build_config.hpp>

#if !GE_WITH_EDITOR
#    include <engine_main.hpp>
#else
#    include <editor_main.hpp>
#endif // !WITH_EDITOR

#include <ecs/component_serialization.hpp>
#include <filesystem>
#include <module_loader.hpp>
#include <paths_internal.hpp>
#include <stdexcept>
#include <string>
#include <string_view>

int main(int argc, char** argv) {
    // Required arguments: path to executable and path to the project file
    // if (argc < 2) {
    //     throw std::runtime_error("Missing path to project file");
    // }

    // if(!std::filesystem::exists(argv[1])) {
    //     throw std::runtime_error("Specified project file does not exist");
    // }

    std::filesystem::path exe_directory(argv[0], std::filesystem::path::generic_format);
    exe_directory.remove_filename();
    paths::set_engine_executable_directory(exe_directory);
    //std::filesystem::path project_directory(argv[1], std::filesystem::path::generic_format);
    std::filesystem::path project_directory("C:/Users/An0num0us/Documents/GameEngine_Game/GameEngine_Game.geproject", std::filesystem::path::generic_format);
    project_directory.remove_filename();
    paths::set_project_directory(project_directory);

    // TODO Validate project file

#if GE_DEBUG
    char const* game_module_name = "GameEngine_Gamed.dll";
#else
    char const* game_module_name = "GameEngine_Game.dll";
#endif
    Module game_module = load_module(game_module_name);
    get_component_serialization_funcs = get_function_from_module<get_component_serialization_funcs_t>(game_module, "get_component_serialization_functions");

#if !GE_WITH_EDITOR
    engine_main();
#else
    editor_main();
#endif // !WITH_EDITOR

    unload_module(game_module);

    return 0;
}