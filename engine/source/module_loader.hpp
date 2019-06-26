#ifndef MODULE_LOADER_HPP_INCLUDE
#define MODULE_LOADER_HPP_INCLUDE

#ifdef _MSC_VER
#    pragma once
#endif // _MSC_VER

#include <string_view>

class Module {
public:
    void* handle;
};

Module load_module(std::string_view module_name);
void unload_module(Module module);
void* get_function_from_module(Module module, std::string_view name);

template <typename T>
T get_function_from_module(Module module, std::string_view name) {
    return static_cast<T>(get_function_from_module(module, name));
}

#endif // !MODULE_LOADER_HPP_INCLUDE
