#ifndef CORE_ATL_DETAIL_TRAITS_TRANSFORMATIONS_HPP_INCLUDE
#define CORE_ATL_DETAIL_TRAITS_TRANSFORMATIONS_HPP_INCLUDE

#include <core/types.hpp>
#include <core/atl/detail/traits_base.hpp>
#include <core/atl/detail/traits_common.hpp>
#include <core/atl/detail/traits_function.hpp>

namespace anton_engine::atl {
    // Type_Identity
    //
    template <typename T>
    struct Type_Identity {
        using type = T;
    };

    template <typename T>
    using type_identity = typename Type_Identity<T>::type;

    // Remove_Extent
    //
    template <typename T>
    struct Remove_Extent {
        using type = T;
    };

    template <typename T>
    struct Remove_Extent<T[]> {
        using type = T;
    };

    template <typename T, usize N>
    struct Remove_Extent<T[N]> {
        using type = T;
    };

    template <typename T>
    using remove_extent = typename Remove_Extent<T>::type;

    // Remove_All_Extents
    //
    template <typename T>
    struct Remove_All_Extents {
        using type = T;
    };

    template <typename T>
    struct Remove_All_Extents<T[]> {
        using type = typename Remove_All_Extents<T>::type;
    };

    template <typename T, usize N>
    struct Remove_All_Extents<T[N]> {
        using type = typename Remove_All_Extents<T>::type;
    };

    template <typename T>
    using remove_all_extents = typename Remove_All_Extents<T>::type;

    template <typename T>
    struct Remove_Reference {
        using type = T;
    };

    template <typename T>
    struct Remove_Reference<T&> {
        using type = T;
    };

    template <typename T>
    struct Remove_Reference<T&&> {
        using type = T;
    };

    template <typename T>
    using remove_reference = typename Remove_Reference<T>::type;

    template <typename T>
    struct Remove_Pointer {
        using type = T;
    };

    template <typename T>
    struct Remove_Pointer<T*> {
        using type = T;
    };

    template <typename T>
    struct Remove_Pointer<T* const> {
        using type = T;
    };

    template <typename T>
    using remove_pointer = typename Remove_Pointer<T>::type;

    template <typename T>
    struct Remove_Const {
        using type = T;
    };

    template <typename T>
    struct Remove_Const<T const> {
        using type = T;
    };

    template <typename T>
    using remove_const = typename Remove_Const<T>::type;

    template <typename T>
    struct Remove_Const_Ref {
        using type = remove_const<remove_reference<T>>;
    };

    template <typename T>
    using remove_const_ref = typename Remove_Const_Ref<T>::type;

    namespace detail {
        template <typename T, typename = void>
        struct Add_Reference {
            using lvalue = T;
            using rvalue = T;
        };

        template <typename T>
        struct Add_Reference<T, void_trait<T&>> {
            using lvalue = T&;
            using rvalue = T&&;
        };
    } // namespace detail

    // Add_Lvalue_Reference
    //
    template <typename T>
    struct Add_Lvalue_Reference {
        using type = typename detail::Add_Reference<T>::lvalue;
    };

    template <typename T>
    using add_lvalue_reference = typename Add_Lvalue_Reference<T>::type;

    // Add_Rvalue_Reference
    //
    template <typename T>
    struct Add_Rvalue_Reference {
        using type = typename detail::Add_Reference<T>::rvalue;
    };

    template <typename T>
    using add_rvalue_reference = typename Add_Rvalue_Reference<T>::type;

    // Add_Pointer
    //
    template <typename T>
    struct Add_Pointer {
    private:
        template <typename U>
        static auto test(int) -> atl::Type_Identity<atl::remove_reference<T>*>;

        template <typename U>
        static auto test(...) -> atl::Type_Identity<T>;

    public:
        using type = typename decltype(test<T>(0))::type;
    };

    template <typename T>
    using add_pointer = typename Add_Pointer<T>::type;

    template <typename T>
    struct Add_Const {
        using type = T const;
    };

    template <typename T>
    using add_const = typename Add_Const<T>::type;

    // Decay
    //
    template <typename T>
    struct Decay {
    private:
        using U = atl::remove_reference<T>;

    public:
        using type = atl::conditional<atl::detail::Is_Array<U>::value, atl::remove_extent<U>*,
                                            atl::conditional<atl::is_function<T>, atl::add_pointer<T>, atl::remove_const<T>>>;
    };

    template <typename T>
    using decay = typename Decay<T>::type;

    // Make_Signed
    //
    template <typename T>
    struct Make_Signed {};
    // clang-format off
    template<> struct Make_Signed<signed char> { using type = signed char; };
    template<> struct Make_Signed<unsigned char> { using type = signed char; };
    template<> struct Make_Signed<signed short> { using type = signed short; };
    template<> struct Make_Signed<unsigned short> { using type = signed short; };
    template<> struct Make_Signed<signed int> { using type = signed int; };
    template<> struct Make_Signed<unsigned int> { using type = signed int; };
    template<> struct Make_Signed<signed long> { using type = signed long; };
    template<> struct Make_Signed<unsigned long> { using type = signed long; };
    template<> struct Make_Signed<signed long long> { using type = signed long long; };
    template<> struct Make_Signed<unsigned long long> { using type = signed long long; };
    // clang-format on

    template <typename T>
    using make_signed = typename Make_Signed<T>::type;

    // Make_Unsigned
    //
    template <typename T>
    struct Make_Unsigned {};
    // clang-format off
    template<> struct Make_Unsigned<signed char> { using type = unsigned char; };
    template<> struct Make_Unsigned<unsigned char> { using type = unsigned char; };
    template<> struct Make_Unsigned<signed short> { using type = unsigned short; };
    template<> struct Make_Unsigned<unsigned short> { using type = unsigned short; };
    template<> struct Make_Unsigned<signed int> { using type = unsigned int; };
    template<> struct Make_Unsigned<unsigned int> { using type = unsigned int; };
    template<> struct Make_Unsigned<signed long> { using type = unsigned long; };
    template<> struct Make_Unsigned<unsigned long> { using type = unsigned long; };
    template<> struct Make_Unsigned<signed long long> { using type = unsigned long long; };
    template<> struct Make_Unsigned<unsigned long long> { using type = unsigned long long; };
    // clang-format on

    template <typename T>
    using make_unsigned = typename Make_Unsigned<T>::type;
} // namespace anton_engine::atl

#endif // !CORE_ATL_DETAIL_TRAITS_TRANSFORMATIONS_HPP_INCLUDE
