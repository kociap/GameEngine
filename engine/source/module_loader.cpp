#include <module_loader.hpp>

#include <stdexcept>
#include <string>

#ifdef _MSC_VER
#    include <windows.h>
#else
#    include <dlfcn.h>
#endif // _MSC_VER

Module load_module(std::string_view module_name) {
    Module module;
#ifdef _MSC_VER
    module.handle = LoadLibraryA(module_name.data());
#else
    module.handle = dlopen(module_name.data(), RTLD_LAZY)
#endif
    if (!module.handle) {
        throw std::runtime_error(std::string("Could not load module ") + module_name.data());
    }
	return module;
}

void* get_function_from_module(Module module, std::string_view name) {
#ifdef _MSC_VER
    void* func_ptr = GetProcAddress(static_cast<HMODULE>(module.handle), name.data());
#else
    void* func_ptr = dlsym(module.handle, name.data());
#endif

    if (!func_ptr) {
        throw std::runtime_error(std::string("Could not load function ") + name.data());
    }
    return func_ptr;
}

void unload_module(Module module) {
#ifdef _MSC_VER
    if (!FreeLibrary(static_cast<HMODULE>(module.handle))) {
        throw std::runtime_error("Could not unload module");
    }
#else
    if (dlclose(module.handle)) {
        throw std::runtime_error("Could not unload module");
    }
#endif
}