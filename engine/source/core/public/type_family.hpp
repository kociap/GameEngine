#ifndef CORE_TYPE_FAMILY_HPP_INCLUDE
#define CORE_TYPE_FAMILY_HPP_INCLUDE

#include <config.hpp>
#include <string_view>
#include <type_traits>

namespace anton_engine {
    class Type_Family {
    public:
        using family_t = id_type;

        template <typename... Ts>
        static family_t family_id() {
            static family_t hash = hash_types<std::decay_t<Ts>...>();
            return hash;
        }

    private:
        template <typename... Ts>
        static family_t hash_types() {
            auto hasher = std::hash<std::string_view>();
            return hasher(get_types<Ts...>());
        }

        template <typename... Ts>
        static std::string_view get_types() {
            // TODO use only types instead of the entire signature
#if defined(__clang__) || defined(__GNUC__)
            std::string_view signature = __PRETTY_FUNCTION__;
#elif defined(_MSC_VER)
            // return_type calling_convention func_name<template_parameters>(arguments)
            std::string_view signature = __FUNCSIG__;
#else
            static_assert(false, "Compiling with unknown compiler. Cannot stringify template arguments");
#endif
            return signature;
        }
    };
} // namespace anton_engine

#endif // !CORE_TYPE_FAMILY_HPP_INCLUDE
