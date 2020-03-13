#ifndef CORE_TYPEID_HPP_INCLUDE
#define CORE_TYPEID_HPP_INCLUDE

#include <core/types.hpp>
#include <core/atl/string_view.hpp>

namespace anton_engine {
    template <typename... Ts>
    u64 type_identifier() {
        // TODO use only types instead of the entire signature
#if defined(__clang__) || defined(__GNUC__)
        atl::String_View const signature = __PRETTY_FUNCTION__;
#elif defined(_MSC_VER)
        // return_type calling_convention func_name<template_parameters>(arguments)
        atl::String_View const signature = __FUNCSIG__;
#else
        static_assert(false, "Compiling with unknown compiler. Cannot stringify template arguments");
#endif

        return atl::hash(signature);
    }
} // namespace anton_engine

#endif // !CORE_TYPEID_HPP_INCLUDE
