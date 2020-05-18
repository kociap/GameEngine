#ifndef RENDERING_FONTS_HPP_INCLUDE
#define RENDERING_FONTS_HPP_INCLUDE

#include <core/atl/slice.hpp>
#include <core/atl/string_view.hpp>
#include <core/atl/vector.hpp>
#include <core/types.hpp>

namespace anton_engine::rendering {
    class Font_Face;

    class Font_Render_Info {
    public:
        u32 points;
        u32 h_dpi;
        u32 v_dpi;
    };

    // All metrics are expressed in font units.
    //
    class Face_Metrics {
    public:
        atl::String_View family_name;
        atl::String_View style_name;
        // The typographic ascender of the face.
        i32 ascender;
        // The typographic descender of the face (negative if below baseline).
        i32 descender;
        // Distance between 2 consecutive baselines.
        i32 line_height;
        i32 glyph_x_max;
        i32 glyph_x_min;
        i32 glyph_y_max;
        i32 glyph_y_min;
        i32 max_advance;
        // font units per em
        i32 units_per_em;
    };

    // All metrics are expressed in 26.6 pixel format (1/64 of a pixel precision).
    //
    // Despite FreeType's documentation saying horizontal bearing is always positive for horizontal
    // layout, in practice it may be negative (e.g. letter 'j' in Comic Sans).
    //
    class Glyph_Metrics {
    public:
        // Width of the glyph.
        i32 width;
        // Height of the glyph.
        i32 height;
        // Horizontal offset from the pen position.
        i32 bearing_x;
        // Height above the baseline.
        i32 bearing_y;
        i32 advance;
    };

    class Glyph {
    public:
        Glyph_Metrics metrics;
        // Handle a texture.
        u64 texture;
        // uv coordinates of the glyph in the texture.
        Rect_f32 uv;
    };

    bool init_font_rendering();
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

    // points_to_pixels
    // points - points in 26.6 format (1/64 of a point precision).
    //
    // Returns:
    // Equialent size in pixels in 26.6 format (1/64 of a pixel precision).
    //
    i64 points_to_pixels(i64 points, i64 dpi);

    Face_Metrics get_face_metrics(Font_Face* face);
    // Face_Metrics get_face_metrics_as_pixels(Font_Face* face, Font_Render_Info);

    // compute_text_width
    // Computes the width of the bounding box for text required to render it without clipping.
    // text is required to be drawn on a single line. Newline characters are ignored.
    //
    // Returns:
    // Width of text drawn on a single line with specified face and info in 26.6 pixel format (1/64 of a pixel precision).
    //
    i64 compute_text_width(Font_Face* face, Font_Render_Info info, atl::String_View text);

    atl::Vector<Glyph> render_text(Font_Face* face, Font_Render_Info info, atl::String_View text);
} // namespace anton_engine::rendering

#endif // !RENDERING_FONTS_HPP_INCLUDE
