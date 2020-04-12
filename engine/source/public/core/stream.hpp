#ifndef CORE_STREAM_HPP_INCLUDE
#define CORE_STREAM_HPP_INCLUDE

#include <core/types.hpp>
#include <core/atl/slice.hpp>
#include <core/atl/string_view.hpp>

namespace anton_engine {
    constexpr char32 eof_char32 = (char32)-1;

    enum class Seek_Dir {
        beg,
        cur,
        end,
    };

    enum class Open_Mode {
        binary, 
        append,
    };

    class Output_Stream {
    public:
        virtual ~Output_Stream() {}

        [[nodiscard]] virtual operator bool() const = 0;

        virtual void flush() = 0;
        virtual void write(void const* buffer, i64 count) = 0;
        virtual void write(atl::Slice<u8 const> buffer) = 0;
        virtual void write(atl::String_View buffer) = 0;
        virtual void put(char32) = 0;
        virtual void seek(Seek_Dir dir, i64 offset) = 0;
        virtual i64 tell() = 0;
    };

    class Input_Stream {
    public:
        virtual ~Input_Stream() {}

        [[nodiscard]] virtual operator bool() const = 0;

        virtual void read(void* buffer, i64 count) = 0;
        virtual void read(atl::Slice<u8> buffer) = 0;
        virtual char32 peek() = 0;
        virtual char32 get() = 0;
        virtual void unget(char32) = 0;
        virtual void seek(Seek_Dir dir, i64 offset) = 0;
        virtual i64 tell() = 0;
    };
}

#endif // !CORE_STREAM_HPP_INCLUDE
