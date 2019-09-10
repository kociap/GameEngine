#ifndef MODULE_LOADER_HPP_INCLUDE
#define MODULE_LOADER_HPP_INCLUDE

#ifdef _MSC_VER
#    pragma once
#endif // _MSC_VER

#include <anton_stl/string_view.hpp>

namespace anton_engine {
    class Module {
    public:
        void* handle;
    };

    Module load_module(anton_stl::String_View module_name);
    void unload_module(Module module);
    void* get_function_from_module(Module module, anton_stl::String_View name);

    template <typename T>
    T get_function_from_module(Module module, anton_stl::String_View name) {
        return static_cast<T>(get_function_from_module(module, name));
    }
} // namespace anton_engine

#endif // !MODULE_LOADER_HPP_INCLUDE
