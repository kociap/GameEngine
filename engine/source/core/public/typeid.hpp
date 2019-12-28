#ifndef CORE_TYPEID_HPP_INCLUDE
#define CORE_TYPEID_HPP_INCLUDE

#include <anton_int.hpp>
#include <anton_stl/string_view.hpp>

namespace anton_engine {
    template <typename... Ts>
    u64 type_identifier() {
        // TODO use only types instead of the entire signature
#if defined(__clang__) || defined(__GNUC__)
        anton_stl::String_View const signature = __PRETTY_FUNCTION__;
#elif defined(_MSC_VER)
        // return_type calling_convention func_name<template_parameters>(arguments)
        anton_stl::String_View const signature = __FUNCSIG__;
#else
        static_assert(false, "Compiling with unknown compiler. Cannot stringify template arguments");
#endif

        return anton_stl::hash(signature);
    }
} // namespace anton_engine

#endif // !CORE_TYPEID_HPP_INCLUDE
