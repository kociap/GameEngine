#ifndef DLL_BUILD_CONFIG_HPP_INCLUDE
#define DLL_BUILD_CONFIG_HPP_INCLUDE

// TODO add visibility in gcc https://gcc.gnu.org/wiki/Visibility

#define NO_NAME_MANGLE extern "C"

#if defined(_MSC_VER)
#    define DLL_EXPORT __declspec(dllexport)
#    define DLL_IMPORT __declspec(dllimport)
#    if defined(COMPILING_DLL)
#        define DLL_API DLL_EXPORT
#        define DLL_EXTERNAL_API DLL_IMPORT
#    else
#        define DLL_API DLL_IMPORT
#        define DLL_EXTERNAL_API DLL_EXPORT
#    endif
#else
#    define DLL_API
#    define DLL_EXPORT
#    define DLL_IMPORT
#endif
#endif // !DLL_BUILD_CONFIG_HPP_INCLUDE
