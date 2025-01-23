#pragma once

#ifdef _MSC_VER
  #pragma once
#endif // _MSC_VER

#include <anton/string_view.hpp>

namespace anton_engine {
  class Module {
  public:
    void* handle;
  };

  Module load_module(anton::String_View module_name);
  void unload_module(Module module);
  void* get_function_from_module(Module module, anton::String_View name);

  template<typename T>
  T get_function_from_module(Module module, anton::String_View name)
  {
    return reinterpret_cast<T>(get_function_from_module(module, name));
  }
} // namespace anton_engine
