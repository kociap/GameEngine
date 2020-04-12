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

    class Output_File_Stream: public Output_Stream {
    public:
        Output_File_Stream();
        explicit Output_File_Stream(atl::String const& filename);
        Output_File_Stream(Output_File_Stream const&) = delete;
        Output_File_Stream(Output_File_Stream&&);
        Output_File_Stream& operator=(Output_File_Stream const&) = delete;
        Output_File_Stream& operator=(Output_File_Stream&&);
        virtual ~Output_File_Stream();

        [[nodiscard]] virtual operator bool() const override;

        bool open(atl::String const& filename);
        void close();
        [[nodiscard]] bool is_open() const;

        virtual void flush() override;
        virtual void write(void const* buffer, i64 count) override;
        virtual void write(atl::Slice<u8 const> buffer) override;
        virtual void write(atl::String_View buffer) override;
        virtual void put(char32) override;
        virtual void seek(Seek_Dir dir, i64 offset) override;
        virtual i64 tell() override;
    
    private:
        void* _buffer;
    };

    class Input_File_Stream: public Input_Stream {
    public:
        Input_File_Stream();
        explicit Input_File_Stream(atl::String const& filename);
        Input_File_Stream(Input_File_Stream const&) = delete;
        Input_File_Stream(Input_File_Stream&&);
        Input_File_Stream& operator=(Input_File_Stream const&) = delete;
        Input_File_Stream& operator=(Input_File_Stream&&);
        virtual ~Input_File_Stream();

        [[nodiscard]] virtual operator bool() const override;

        bool open(atl::String const& filename);
        void close();
        [[nodiscard]] bool is_open() const;

        virtual void read(void* buffer, i64 count) override;
        virtual void read(atl::Slice<u8> buffer) override;
        virtual char32 peek() override;
        virtual char32 get() override;
        virtual void unget(char32) override;
        virtual void seek(Seek_Dir dir, i64 offset) override;
        virtual i64 tell() override;

    private:
        void* _buffer;
    };
} // namespace anton_engine::fs

#endif // !CORE_FILESYSTEM_HPP_INCLUDE
