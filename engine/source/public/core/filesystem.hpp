#ifndef CORE_FILESYSTEM_HPP_INCLUDE
#define CORE_FILESYSTEM_HPP_INCLUDE

#include <core/atl/string_view.hpp>
#include <core/atl/string.hpp>
#include <core/stream.hpp>

namespace anton_engine::fs {
    atl::String concat_paths(atl::String_View path1, atl::String_View path2);
    atl::String_View remove_filename(atl::String_View path);
    atl::String_View remove_extension(atl::String_View path);
    atl::String_View get_filename(atl::String_View path);
    atl::String_View get_filename_no_extension(atl::String_View path);
    atl::String_View get_extension(atl::String_View path);
    atl::String_View get_directory_name(atl::String_View path);
    bool has_extension(atl::String_View path);
    bool has_filename(atl::String_View path);

    class Input_File_Stream: public Input_Stream {
    public:
        Input_File_Stream();
        Input_File_Stream(atl::String_View filename);
        Input_File_Stream(Input_File_Stream const&) = delete;
        Input_File_Stream(Input_File_Stream&&);
        Input_File_Stream& operator=(Input_File_Stream const&) = delete;
        Input_File_Stream& operator=(Input_File_Stream&&);
        virtual ~Input_File_Stream();

        void open(atl::String_View filename);
        void close();

        virtual void read(void* buffer, i64 count) override;
        virtual char32 peek() override;
        virtual char32 get() override;
        virtual void unget(char32) override;
        virtual void seek(Seek_Dir dir, i64 offset) override;
        virtual i64 tell() override;

    private:
        void* _buffer;
    };

    class Output_File_Stream: public Output_Stream {
    public:
        Output_File_Stream();
        Output_File_Stream(atl::String_View filename);
        Output_File_Stream(Output_File_Stream const&) = delete;
        Output_File_Stream(Output_File_Stream&&);
        Output_File_Stream& operator=(Output_File_Stream const&) = delete;
        Output_File_Stream& operator=(Output_File_Stream&&);
        virtual ~Output_File_Stream();

        void open(atl::String_View filename);
        void close();

        virtual void flush() override;
        virtual void write(void const* buffer, i64 count) override;
        virtual void put(char32) override;
        virtual void seek(Seek_Dir dir, i64 offset) override;
        virtual i64 tell() override;
    
    private:
        void* _buffer;
    };
} // namespace anton_engine::fs

#endif // !CORE_FILESYSTEM_HPP_INCLUDE
