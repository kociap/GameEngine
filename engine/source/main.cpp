#include <filesystem>
#include <module_loader.hpp>
#include <paths_internal.hpp>
#include <stdexcept>
#include <string>

#include <ecs/system_management.hpp>
#include <ecs/component_serialization.hpp>

#include <build_config.hpp>
namespace anton_engine {
#if ANTON_WITH_EDITOR

    int editor_main();
#else
    int engine_main();
#endif // !ANTON_WITH_EDITOR
} // namespace anton_engine

int main(int argc, char** argv) {
    using namespace anton_engine;
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
    std::filesystem::path project_directory("C:/Users/An0num0us/Documents/GameEngine_Game/GameEngine_Game.geproject", std::filesystem::path::generic_format);
    project_directory.remove_filename();
    paths::set_project_directory(project_directory);

    // TODO Validate project file

#if ANTON_DEBUG
    char const* game_module_name = "GameEngine_Gamed.dll";
#else
    char const* game_module_name = "GameEngine_Game.dll";
#endif // ANTON_DEBUG
    Module game_module = load_module(game_module_name);
    get_component_serialization_funcs = get_function_from_module<get_component_serialization_funcs_t>(game_module, "get_component_serialization_functions");
    create_systems = get_function_from_module<create_systems_type>(game_module, "create_systems");

#if !ANTON_WITH_EDITOR
    engine_main();
#else
    editor_main();
#endif // !ANTON_WITH_EDITOR

    unload_module(game_module);

    return 0;
}
