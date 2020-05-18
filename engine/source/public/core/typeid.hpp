#ifndef CORE_TYPEID_HPP_INCLUDE
#define CORE_TYPEID_HPP_INCLUDE

#include <core/atl/string_view.hpp>
#include <core/types.hpp>

namespace anton_engine {
    template<typename T>
    constexpr atl::String_View type_name() {
        // TODO: Fix.
#if defined(__clang__) || defined(__GNUC__)
        constexpr atl::String_View p = __PRETTY_FUNCTION__;
        // TODO: Adding u8 (u8" = ") makes the literal a char8_t string literal in C++17 (???)
        constexpr i64 type_offset = atl::find_substring(p, u8" = ");
        return atl::String_View{p.data() + type_offset + 3, p.data() + p.size_bytes() - 1};
#elif defined(_MSC_VER)
        constexpr atl::String_View p = __FUNCSIG__;
        return atl::String_View(p.data() + 84, p.size_bytes() - 84 - 7);
#else
#    error Compiling with unknown compiler. Cannot stringify template arguments
#endif
    }

    using Type_Identifier = u64;

    template<typename... Ts>
    constexpr Type_Identifier type_identifier() {
        constexpr atl::String_View name = type_name<Ts...>();
        return atl::hash(name);
    }
} // namespace anton_engine

#endif // !CORE_TYPEID_HPP_INCLUDE
