#include <filesystem>
#include <module_loader.hpp>
#include <paths_internal.hpp>
#include <stdexcept>

#include <ecs/component_serialization.hpp>
#include <ecs/system_management.hpp>

#include <build_config.hpp>

namespace anton_engine {
#if ANTON_WITH_EDITOR
    int editor_main(int argc, char** argv);
#else
    int engine_main(int argc, char** argv);
#endif // !ANTON_WITH_EDITOR
} // namespace anton_engine

int main(int argc, char** argv) {
    using namespace anton_engine;

#if ANTON_DEBUG
    char const* game_module_name = "GameEngine_Gamed.dll";
#else
    char const* game_module_name = "GameEngine_Game.dll";
#endif // ANTON_DEBUG
    Module game_module = load_module(game_module_name);
    get_component_serialization_funcs = get_function_from_module<get_component_serialization_funcs_t>(game_module, "get_component_serialization_functions");
    create_systems = get_function_from_module<create_systems_type>(game_module, "create_systems");

#if !ANTON_WITH_EDITOR
    engine_main(argc, argv);
#else
    editor_main(argc, argv);
#endif // !ANTON_WITH_EDITOR

    unload_module(game_module);

    return 0;
}
