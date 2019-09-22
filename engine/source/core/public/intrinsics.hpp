#ifndef CORE_INTRINSICS_HPP_INCLUDE
#define CORE_INTRINSICS_HPP_INCLUDE

#define ANTON_UNUSED(x) ((void)(x))

#if defined(__clang__) || defined(__GNUC__)
#    define ANTON_UNREACHABLE() __builtin_unreachable()
#    define ANTON_UNLIKELY(x) __builtin_expect(!!(x), 0)
#    define ANTON_LIKELY(x) __builtin_expect(!!(x), 1)
#elif defined(_MSC_VER)
#    define ANTON_UNREACHABLE() __assume(0)
#    define ANTON_UNLIKELY(x) x
#    define ANTON_EXPECT(x) x
#else
#    define ANTON_UNREACHABLE()
#    define ANTON_UNLIKELY(x) x
#    define ANTON_EXPECT(x) x
#endif

#endif // !CORE_INTRINSICS_HPP_INCLUDE
