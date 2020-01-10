#ifndef CORE_ASSERT_HPP_INCLUDE
#define CORE_ASSERT_HPP_INCLUDE

#include <build_config.hpp>
#include <intrinsics.hpp>

namespace anton_engine {
    void anton_assert(char const* message, char const* file, unsigned long long line);

#define ANTON_FAIL(condition, msg) (ANTON_LIKELY(static_cast<bool>(condition)) ? (void)0 : ::anton_engine::anton_assert(msg, __FILE__, __LINE__))

#if ANTON_DEBUG
#    define ANTON_ASSERT(condition, msg) (ANTON_LIKELY(static_cast<bool>(condition)) ? (void)0 : ::anton_engine::anton_assert(msg, __FILE__, __LINE__))
#else
#    define ANTON_ASSERT(condition, msg) ((void)0)
#endif

#ifndef ANTON_DISABLE_VERIFY
#    define ANTON_DISABLE_VERIFY 0
#endif

#if !ANTON_DISABLE_VERIFY
// ANTON_VERIFY
// Debug and Release builds assert. Must be disabled explicitly.
#    define ANTON_VERIFY(condition, msg) (ANTON_LIKELY(static_cast<bool>(condition)) ? (void)0 : ::anton_engine::anton_assert(msg, __FILE__, __LINE__))
#else
#    define ANTON_VERIFY() ((void)0)
#endif // !ANTON_DISABLE_VERIFY
} // namespace anton_engine

#endif // !CORE_ASSERT_HPP_INCLUDE
