#ifndef CORE_WINDOW_DEBUGGING_HPP_INCLUDE
#define CORE_WINDOW_DEBUGGING_HPP_INCLUDE

#include <anton_stl/string.hpp>
#include <cstdint>

namespace anton_engine::windows::debugging {
    // SymTagFunction, SymTagBlock, SymTagData, SymTagAnnotation, SymTagLabel, SymTagPublicSymbol, SymTagUDT, SymTagEnum, SymTagFunctionType, SymTagPointerType,
    //     SymTagArrayType, SymTagBaseType, SymTagTypedef, SymTagBaseClass, SymTagFriend, SymTagFunctionArgType, SymTagFuncDebugStart, SymTagFuncDebugEnd,
    //     SymTagUsingNamespace,

    enum class Base_Type {
        unknown_type,
        void_type,
        bool_type,
        char_type,
        char16,
        char32,
        wchar,
        int8,
        int16,
        int32,
        int64,
        uint8,
        uint16,
        uint32,
        uint64,
        float32,
        float64,
    };

    enum class Symbol_Tag {
        function_type,
        function_argument_type,
        pointer_type,
        base_type,
        udt,
    };

    class Base_Type_Info {
    public:
        Base_Type type;
    };

    class Function_Argument_Info {
    public:
    };

    class Function_Type_Info {
    public:
        anton_stl::String name;
    };

    class Type_Info {
    public:
        Symbol_Tag tag;
        union {
            Base_Type_Info base_type;
        } info;
    };

    [[nodiscard]] anton_stl::String get_type_as_string(void* process_handle, uint64_t module_base, uint64_t index);
} // namespace anton_engine::windows::debugging

#endif // !CORE_WINDOW_DEBUGGING_HPP_INCLUDE
