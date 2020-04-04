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

        i32 hori_bearing_x;
        i32 hori_bearing_y;
        i32 hori_advance;

        // Vertical layout
        // i32 vert_bearing_x;
        // i32 vert_bearing_y;
        // i32 vert_advance;

        u32 bitmap_row_width; // in bytes
        atl::Vector<u8> bitmap;
    };

    // All metrics are expressed in font units.
    //
    class Face_Metrics {
    public:
        atl::String_View family_name;
        atl::String_View style_name;
        // Max height above baseline.
        i32 ascent;
        // Max height below baseline (negative if below baseline). 
        i32 descent;
        // Distance between 2 consecutive baselines.
        i32 line_height;
        i32 max_advance;
        // font units per em
        i32 units_per_em;
    };  

    class Text_Metrics {
    public:
        i32 width;
        i32 height;
        i32 baseline;
    };

    class Text_Image {
    public:
        u32 texture;
        i32 width;
        i32 height;
        i32 baseline;
    };

    void init_font_rendering();
    void terminate_font_rendering();

    // Returns:
    // Strongly typed handle that uniquely identifies the face or nullptr if failed to load the face.
    //
    Font_Face* load_face_from_file(atl::String_View path, u32 face_index);

    // Returns:
    // Strongly typed handle that uniquely identifies the face or nullptr if failed to load the face.
    //
    Font_Face* load_face(atl::Slice<u8 const> file_data, u32 face_index);
    void unload_face(Font_Face* face);

    Face_Metrics get_face_metrics(Font_Face* face);
    // Face_Metrics get_face_metrics_as_pixels(Font_Face* face, Font_Render_Info)

    atl::Vector<Glyph> rasterize_text_glyphs(Font_Face* face, Font_Render_Info info, atl::String_View string);
    Text_Metrics compute_text_dimensions(Font_Face* face, Font_Render_Info info, atl::String_View string);
    Text_Image render_text(Font_Face* face, Font_Render_Info info, atl::String_View string);
}

#endif // !RENDERING_FONTS_HPP_INCLUDE
