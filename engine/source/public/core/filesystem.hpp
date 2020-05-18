#ifndef CORE_FILESYSTEM_HPP_INCLUDE
#define CORE_FILESYSTEM_HPP_INCLUDE

#include <core/atl/string.hpp>
#include <core/atl/string_view.hpp>
#include <core/stream.hpp>

namespace anton_engine::fs {
    // normalize_path
    //
    atl::String normalize_path(atl::String_View path);

    // concat_paths
    // Concatenate paths with separator.
    //
    atl::String concat_paths(atl::String_View path1, atl::String_View path2);

    atl::String_View remove_filename(atl::String_View path);
    atl::String_View remove_extension(atl::String_View path);
    atl::String_View get_filename(atl::String_View path);
    atl::String_View get_filename_no_extension(atl::String_View path);
    atl::String_View get_extension(atl::String_View path);

    atl::String parent_path(atl::String_View const path);

    // get_directory_name
    // Does not support paths using "file:".
    //
    atl::String_View get_directory_name(atl::String_View path);

    // get_last_write_time
    // Returns time in ms since 1970 (TODO: document exact time) of the last modification of a file.
    // TODO: Currently returns gods know what time.
    //
    i64 get_last_write_time(atl::String_View path);

    bool has_extension(atl::String_View path);
    bool has_filename(atl::String_View path);
    bool exists(atl::String_View path);

    enum class Open_Mode {
        // Has effect only on Windows. Makes all reading operations translate the \n\r sequences into \n.
        // Opposite of std::ios_base::open_mode::binary.
        windows_translate_newline,
    };

    class Output_File_Stream: public Output_Stream {
    public:
        Output_File_Stream();
        explicit Output_File_Stream(atl::String const& filename);
        explicit Output_File_Stream(atl::String const& filename, Open_Mode open_mode);
        Output_File_Stream(Output_File_Stream const&) = delete;
        Output_File_Stream(Output_File_Stream&&);
        Output_File_Stream& operator=(Output_File_Stream const&) = delete;
        Output_File_Stream& operator=(Output_File_Stream&&);
        virtual ~Output_File_Stream();

        [[nodiscard]] virtual operator bool() const override;

        bool open(atl::String const& filename);
        bool open(atl::String const& filename, Open_Mode open_mode);
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
        void* _buffer = nullptr;
    };

    class Input_File_Stream: public Input_Stream {
    public:
        Input_File_Stream();
        explicit Input_File_Stream(atl::String const& filename);
        explicit Input_File_Stream(atl::String const& filename, Open_Mode open_mode);
        Input_File_Stream(Input_File_Stream const&) = delete;
        Input_File_Stream(Input_File_Stream&&);
        Input_File_Stream& operator=(Input_File_Stream const&) = delete;
        Input_File_Stream& operator=(Input_File_Stream&&);
        virtual ~Input_File_Stream();

        [[nodiscard]] virtual operator bool() const override;

        bool open(atl::String const& filename);
        bool open(atl::String const& filename, Open_Mode open_mode);
        void close();
        [[nodiscard]] bool is_open() const;

        virtual void read(void* buffer, i64 count) override;
        virtual void read(atl::Slice<u8> buffer) override;
        virtual char32 peek() override;
        virtual char32 get() override;
        virtual void unget(char32) override;
        virtual void seek(Seek_Dir dir, i64 offset) override;
        virtual i64 tell() override;

        // eof
        // Checks whether the stream has reached end-of-file.
        //
        bool eof() const;

        // error
        // Checks whether the stream has encountered any errors.
        //
        bool error() const;

    private:
        void* _buffer = nullptr;
    };
} // namespace anton_engine::fs

#endif // !CORE_FILESYSTEM_HPP_INCLUDE
