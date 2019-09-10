#include <module_loader.hpp>

#include <stdexcept>
#include <string>

#ifdef _MSC_VER
#    define WIN32_LEAN_AND_MEAN
#    include <Windows.h>
#else
#    include <dlfcn.h>
#endif // _MSC_VER

namespace anton_engine {
    Module load_module(anton_stl::String_View module_name) {
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

    void* get_function_from_module(Module module, anton_stl::String_View name) {
#ifdef _MSC_VER
        // Since GetProcAddress returns a function pointer we reinterpret_cast to void* to silence -Wmicrosoft-cast.
        void* func_ptr = reinterpret_cast<void*>(GetProcAddress(static_cast<HMODULE>(module.handle), name.data()));
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
} // namespace anton_engine
