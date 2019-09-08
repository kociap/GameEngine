#ifndef CORE_ASSERT_HPP_INCLUDE
#define CORE_ASSERT_HPP_INCLUDE

#include <build_config.hpp>

namespace anton_engine {
    void anton_assert(char const* message, char const* file, unsigned long long line);

#if ANTON_DEBUG
#    define ANTON_ASSERT(condition, msg) (static_cast<bool>(condition) ? (void)0 : anton_engine::anton_assert(msg, __FILE__, __LINE__))
#else
#    define ANTON_ASSERT(condition, msg) ((void)0)
#endif
} // namespace anton_engine

#endif // !CORE_ASSERT_HPP_INCLUDE