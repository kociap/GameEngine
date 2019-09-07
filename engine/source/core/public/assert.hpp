#ifndef CORE_ASSERT_HPP_INCLUDE
#define CORE_ASSERT_HPP_INCLUDE

#include <build_config.hpp>

namespace anton_engine {
    void anton_assert(char const* message);

#if ANTON_DEBUG
#    define ANTON_ASSERT(condition, msg) (static_cast<bool>(condition) ? (void)0 : (anton_engine::anton_assert(msg), false))
#else
#    define ANTON_ASSERT(condition, msg) ((void)0)
#endif
} // namespace anton_engine

#endif // !CORE_ASSERT_HPP_INCLUDE
