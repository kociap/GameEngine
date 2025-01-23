#include <module_loader.hpp>

#include <core/exception.hpp>
#include <engine/ecs/component_serialization.hpp>
#include <engine/ecs/system_management.hpp>

#include <stdlib.h>

namespace anton_engine {
#if ANTON_WITH_EDITOR
  int editor_main(int argc, char** argv);
#else
  int engine_main(int argc, char** argv);
#endif // !ANTON_WITH_EDITOR
} // namespace anton_engine

int main(int argc, char** argv)
{
  using namespace anton_engine;

  try {
#if ANTON_WITH_EDITOR
    editor_main(argc, argv);
#else
    engine_main(argc, argv);
#endif // !ANTON_WITH_EDITOR
  } catch(Exception const& e) {
    FILE* file = fopen("crash_log.txt", "w");
    fputs("anton_engine crashed with message:", file);
    anton::String_View const msg = e.get_message();
    fputs(msg.data(), file);
    fclose(file);
  } catch(...) {
    FILE* file = fopen("crash_log.txt", "w");
    fputs("anton_engine crashed with message:", file);
    fputs("unknown error", file);
    fclose(file);
  }

  return 0;
}
