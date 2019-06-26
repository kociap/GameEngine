#include <build_config.hpp>

#if !GE_WITH_EDITOR
#    include <engine_main.hpp>
#else
#    include <editor_main.hpp>
#endif // !WITH_EDITOR

#include <ecs/component_serialization.hpp>
#include <module_loader.hpp>

int main(int argc, char** argv) {
#if GE_DEBUG
    char const* game_module_name = "GameEngine_Gamed.dll";
#else
    char const* game_module_name = "GameEngine_Game.dll";
#endif
    Module game_module = load_module(game_module_name);
    get_component_serialization_funcs = get_function_from_module<get_component_serialization_funcs_t>(game_module, "get_component_serialization_functions");

#if !GE_WITH_EDITOR
    engine_main(argc, argv);
#else
    editor_main(argc, argv);
#endif // !WITH_EDITOR

    unload_module(game_module);
}