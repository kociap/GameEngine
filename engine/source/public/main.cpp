#include <module_loader.hpp>
#include <stdexcept>

#include <engine/ecs/component_serialization.hpp>
#include <engine/ecs/system_management.hpp>
#include <core/exception.hpp>

#include <build_config.hpp>

#include <stdio.h>
#include <string.h>

namespace anton_engine {
#if ANTON_WITH_EDITOR
    int editor_main(int argc, char** argv);
#else
    int engine_main(int argc, char** argv);
#endif // !ANTON_WITH_EDITOR
} // namespace anton_engine

int main(int argc, char** argv) {
    using namespace anton_engine;

    char const* const path = argv[0];
    i64 len = strlen(path);
    i64 i = len;
    for(; i >= 0;  i -= 1) {
        if(path[i] == '\\' || path[i] == '/') {
            i += 1;
            break;
        }
    }

#if ANTON_DEBUG
    char const game_module_name[] = "anton_engine_game_d.dll";
#else
    char const game_module_name[] = "anton_engine_game.dll";
#endif // ANTON_DEBUG

    char const* const module_path = (char*)malloc(i + sizeof(game_module_name));
    memcpy((void*)module_path, path, i);
    memcpy((void*)(module_path + i), game_module_name, sizeof(game_module_name));

    Module game_module = load_module(module_path);
    get_component_serialization_funcs = get_function_from_module<get_component_serialization_funcs_t>(game_module, "get_component_serialization_functions");
    create_systems = get_function_from_module<create_systems_type>(game_module, "create_systems");
    free((void*)module_path);

    char const error_log_name[] = "crash_log.txt";
    char const* const log_path = (char*)malloc(i + sizeof(error_log_name));
    memcpy((void*)log_path, path, i);
    memcpy((void*)(log_path + i), error_log_name, sizeof(error_log_name));

    try {
#if ANTON_WITH_EDITOR
        editor_main(argc, argv);
#else
        engine_main(argc, argv);
#endif // !ANTON_WITH_EDITOR
    } catch(Exception& e) {
        FILE* file = fopen(log_path, "w");
        fputs("anton_engine crashed with message:", file);
        fputs(e.get_message().data(), file);
        fclose(file);
    } catch(...) {
        FILE* file = fopen(log_path, "w");
        fputs("anton_engine crashed with message:", file);
        fputs("unknown error", file);
        fclose(file);
    }
    
    free((void*)log_path);

    unload_module(game_module);

    return 0;
}
