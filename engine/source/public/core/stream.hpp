#ifndef CORE_STREAM_HPP_INCLUDE
#define CORE_STREAM_HPP_INCLUDE

#include <core/types.hpp>

namespace anton_engine {
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

        virtual void flush() = 0;
        virtual void write(void const* buffer, i64 count) = 0;
        virtual void put(char32) = 0;
        virtual void seek(Seek_Dir dir, i64 offset) = 0;
        virtual i64 tell() = 0;
    };

    class Input_Stream {
    public:
        virtual ~Input_Stream() {}

        virtual void read(void* buffer, i64 count) = 0;
        virtual char32 peek() = 0;
        virtual char32 get() = 0;
        virtual void unget(char32) = 0;
        virtual void seek(Seek_Dir dir, i64 offset) = 0;
        virtual i64 tell() = 0;
    };
}

#endif // !CORE_STREAM_HPP_INCLUDE
