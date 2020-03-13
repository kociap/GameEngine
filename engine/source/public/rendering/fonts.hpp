#ifndef RENDERING_FONTS_HPP_INCLUDE
#define RENDERING_FONTS_HPP_INCLUDE

#include <core/atl/slice.hpp>
#include <core/types.hpp>
#include <core/atl/string_view.hpp>
#include <core/atl/vector.hpp>

namespace anton_engine::rendering {
    class Font_Face;

    class Font_Render_Info {
    public:
        u32 points;
        u32 h_dpi;
        u32 v_dpi;
    };

    // All metrics are expressed in 26.6 pixel format (1/64 of a pixel).
    //
    class Glyph {
    public:
        i32 width;
        i32 height;

        i32 horiBearingX;
        i32 horiBearingY;
        i32 horiAdvance;

        i32 vertBearingX;
        i32 vertBearingY;
        i32 vertAdvance;

        u32 bitmap_row_width; // in bytes
        atl::Vector<u8> bitmap;
    };

    void init_font_rendering();
    void terminate_font_rendering();

    // Returns:
    // Strongly typed handle that uniquely identifies the face or nullptr if failed to load the face.
    //
    Font_Face* load_face(atl::Slice<u8 const> file_data, u32 face_index);
    void unload_face(Font_Face* face);
    atl::Vector<Glyph> render_face(Font_Face* face, Font_Render_Info info, atl::String_View string);
}

#endif // !RENDERING_FONTS_HPP_INCLUDE
