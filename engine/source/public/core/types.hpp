#ifndef CORE_TYPES_HPP_INCLUDE
#define CORE_TYPES_HPP_INCLUDE

namespace anton_engine {
    using i8 = signed char;
    using i16 = signed short;
    using i32 = signed int;
    using i64 = signed long long;

    using u8 = unsigned char;
    using u16 = unsigned short;
    using u32 = unsigned int;
    using u64 = unsigned long long;

    using f32 = float;
    using f64 = double;

    using isize = i64;
    using usize = u64;

    using char8 = unsigned char;
    using char16 = char16_t;
    using char32 = char32_t;

    class Rect {
    public:
        i32 left;
        i32 top;
        i32 right;
        i32 bottom;
    };
} // namespace anton_engine

#endif // !CORE_TYPES_HPP_INCLUDE
