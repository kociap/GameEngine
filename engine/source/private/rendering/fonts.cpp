#include <rendering/fonts.hpp>

#include <core/atl/fixed_array.hpp>
#include <core/atl/flat_hash_map.hpp>
#include <core/diagnostic_macros.hpp>
#include <core/exception.hpp>
#include <core/math/math.hpp>
#include <core/unicode/common.hpp>

#include <rendering/glad.hpp>
#include <rendering/opengl.hpp>

ANTON_DISABLE_WARNINGS()
#include <ft2build.h>
#include FT_FREETYPE_H
ANTON_RESTORE_WARNINGS()

namespace anton_engine::rendering {
    static constexpr i32 font_texture_width = 4096;
    static constexpr i32 font_texture_height = 4096;
    static constexpr i32 min_font_row_height = 8;
    static constexpr i32 max_row_count = font_texture_height / min_font_row_height;

    struct Font_Row {
        i32 y_start;
        i32 y_end;
        // Offset from the left side of the texture to first free position.
        i32 pen_pos;
    };

    struct Font_Atlas {
        Font_Face* face;
        Font_Render_Info render_info;
        atl::Vector<u32> textures;
        atl::Vector<atl::Fixed_Array<Font_Row, max_row_count>> rows;
        atl::Flat_Hash_Map<char32, Glyph> glyphs;
    };

    class Font_Library {
    public:
        atl::Vector<Font_Atlas> atlases;
    };

    static Font_Atlas* find_atlas_with_params(Font_Library& lib, Font_Face* face, Font_Render_Info const info) {
        for(Font_Atlas& atlas: lib.atlases) {
            Font_Render_Info& atlas_info = atlas.render_info;
            if(atlas.face == face && atlas_info.points == info.points && atlas_info.h_dpi == info.h_dpi && atlas_info.v_dpi == info.v_dpi) {
                return &atlas;
            }
        }
        return nullptr;
    }

    static Font_Library* font_lib = nullptr;
    static FT_Library ft_lib = nullptr;

    bool init_font_rendering() {
        FT_Error const error = FT_Init_FreeType(&ft_lib);
        if(error) {
            return false;
        }

        font_lib = new Font_Library;
        return true;
    }

    void terminate_font_rendering() {
        for(Font_Atlas& atlas: font_lib->atlases) {
            for(u32 const texture: atlas.textures) {
                glDeleteTextures(1, &texture);
            }
        }

        delete font_lib;
        font_lib = nullptr;

        FT_Done_FreeType(ft_lib);
        ft_lib = nullptr;
    }

    Font_Face* load_face_from_file(atl::String_View const path, u32 const face_index) {
        FT_Face face;
        FT_Error const error = FT_New_Face(ft_lib, path.data(), face_index, &face);
        if(error) {
            return nullptr;
        }

        return reinterpret_cast<Font_Face*>(face);
    }

    Font_Face* load_face(atl::Slice<u8 const> const file_data, u32 const face_index) {
        FT_Face face;
        FT_Error const error = FT_New_Memory_Face(ft_lib, file_data.data(), file_data.size(), face_index, &face);
        if(error) {
            return nullptr;
        }

        return reinterpret_cast<Font_Face*>(face);
    }

    void unload_face(Font_Face* face) {
        // TODO: Unload all rasterized glyphs and textures.
        FT_Done_Face(reinterpret_cast<FT_Face>(face));
    }

    i64 points_to_pixels(i64 points, i64 dpi) {
        return points * dpi / 72;
    }

    Face_Metrics get_face_metrics(Font_Face* _face) {
        FT_Face face = reinterpret_cast<FT_Face>(_face);
        Face_Metrics metrics;
        metrics.family_name = face->family_name;
        metrics.style_name = face->style_name;
        metrics.ascender = face->ascender;
        metrics.descender = face->descender;
        metrics.line_height = face->height;
        metrics.glyph_x_max = face->bbox.xMax;
        metrics.glyph_x_min = face->bbox.xMin;
        metrics.glyph_y_max = face->bbox.yMax;
        metrics.glyph_y_min = face->bbox.yMin;
        metrics.max_advance = face->max_advance_width;
        metrics.units_per_em = face->units_per_EM;
        return metrics;
    }

    // Face_Metrics get_face_metrics_as_pixels(Font_Face* _face, Font_Render_Info const info) {
    //     FT_Face face = reinterpret_cast<FT_Face>(_face);
    //     Face_Metrics metrics;
    //     metrics.family_name = face->family_name;
    //     metrics.style_name = face->style_name;
    //     metrics.ascender = face->ascender;
    //     metrics.descender = face->descender;
    //     metrics.line_height = face->height;
    //     metrics.glyph_x_max = face->bbox.xMax;
    //     metrics.glyph_x_min = face->bbox.xMin;
    //     metrics.glyph_y_max = face->bbox.yMax;
    //     metrics.glyph_y_min = face->bbox.yMin;
    //     metrics.max_advance = face->max_advance_width;
    //     metrics.units_per_em = face->units_per_EM;
    //     return metrics;
    // }

    i64 compute_text_width(Font_Face* const _face, Font_Render_Info const info, atl::String_View const string) {
        FT_Face face = reinterpret_cast<FT_Face>(_face);

        if(FT_Set_Char_Size(face, 0, info.points * 64, info.h_dpi, info.v_dpi)) {
            throw Exception(u8"Could not compute text size: failed to set glyph size.");
        }

        i64 const buf_size = unicode::convert_utf8_to_utf32(string.data(), string.size_bytes(), nullptr) / sizeof(char32);
        atl::Vector<char32> text_utf32{atl::reserve, buf_size};
        text_utf32.force_size(buf_size);
        unicode::convert_utf8_to_utf32(string.data(), string.size_bytes(), text_utf32.data());

        i64 width = 0;
        i64 i = 0;
        // Skip all null-terminators and ignore newline.
        for(; i < buf_size && (text_utf32[i] == U'\0' || text_utf32[i] == U'\n'); ++i) {}

        // Special handling for first character.
        if(i < buf_size) {
            char32 const c = text_utf32[i];
            u32 const glyph_index = FT_Get_Char_Index(face, c);
            if(FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT)) {
                throw Exception(u8"Could not compute text size: failed to load the glyph.");
            }

            FT_Glyph_Metrics& metrics = face->glyph->metrics;
            if(i + 1 != buf_size) {
                // Handle the case of negative left side bearing
                width = -math::min((i32)metrics.horiBearingX, 0) + metrics.horiAdvance;
            } else {
                // Handle the case of this being the only glyph.
                if(!atl::is_whitespace(c)) {
                    width = metrics.width;
                } else {
                    width = metrics.horiAdvance;
                }
            }
            ++i;
        }

        for(; i < buf_size; ++i) {
            char32 const c = text_utf32[i];

            // Skip null-terminator because it's non-printable, but rasterizes to rectangle.
            // Ignore newline.
            if(c == U'\0' || c == U'\n') {
                continue;
            }

            u32 const glyph_index = FT_Get_Char_Index(face, c);
            if(FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT)) {
                throw Exception(u8"Could not compute text size: failed to load the glyph.");
            }

            FT_Glyph_Metrics& metrics = face->glyph->metrics;
            if(i + 1 != buf_size) {
                width += metrics.horiAdvance;
            } else {
                if(!atl::is_whitespace(c)) {
                    width += metrics.horiBearingX + metrics.width;
                } else {
                    width += metrics.horiAdvance;
                }
            }
        }

        return width;
    }

    // Does not handle null-terminator or other zero-width characters.
    //
    static Glyph rasterize_glyph(Font_Library& lib, Font_Face* const _face, Font_Render_Info const info, char32 const codepoint) {
        Font_Atlas* atlas = find_atlas_with_params(lib, _face, info);
        // Ensure the container exists.
        if(!atlas) {
            Font_Atlas& _atlas = lib.atlases.emplace_back();
            _atlas.face = _face;
            _atlas.render_info = info;
            atlas = &_atlas;
        }

        if(auto iter = atlas->glyphs.find(codepoint); iter != atlas->glyphs.end()) {
            return iter->value;
        }

        FT_Face face = reinterpret_cast<FT_Face>(_face);
        if(FT_Set_Char_Size(face, 0, info.points * 64, info.h_dpi, info.v_dpi)) {
            throw Exception(u8"Could not render glyph: failed to set glyph size.");
        }

        u32 const glyph_index = FT_Get_Char_Index(face, codepoint);
        if(FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT)) {
            throw Exception(u8"Could not render glyph: failed to load the glyph.");
        }

        if(FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL)) {
            throw Exception(u8"Could not render glyph.");
        }

        FT_Glyph_Metrics& metrics = face->glyph->metrics;
        Glyph glyph;
        glyph.metrics.width = metrics.width;
        glyph.metrics.height = metrics.height;
        glyph.metrics.bearing_x = metrics.horiBearingX;
        glyph.metrics.bearing_y = metrics.horiBearingY;
        glyph.metrics.advance = metrics.horiAdvance;
        FT_Bitmap& bitmap = face->glyph->bitmap;
        i32 const bitmap_width = math::abs(bitmap.pitch);
        i32 const bitmap_height = bitmap.rows;
        unsigned char* bitmap_buffer_begin = (bitmap.pitch > 0 ? bitmap.buffer : bitmap.buffer - bitmap_width * bitmap_height);
        unsigned char* bitmap_buffer_end = (bitmap.pitch > 0 ? bitmap.buffer + bitmap_width * bitmap_height : bitmap.buffer);
        atl::Vector<u8> tex_data{atl::range_construct, bitmap_buffer_begin, bitmap_buffer_end};

        i32 const required_row_height = bitmap_height + 1 + math::min(bitmap_height / 10, 1);
        auto& rows = atlas->rows;
        // Try to find a slot for the glyph in an existing row.
        for(i64 i = 0; i < rows.size(); ++i) {
            for(Font_Row& row: rows[i]) {
                i32 const row_height = row.y_end - row.y_start;
                i32 const available_width = font_texture_width - row.pen_pos;
                if(bitmap_height + 1 <= row_height && bitmap_width <= available_width) {
                    // Copy bitmap to texture.
                    u32 const texture = atlas->textures[i];
                    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
                    glTextureSubImage2D(texture, 0, row.pen_pos, row.y_start, bitmap_width, bitmap_height, GL_RED, GL_UNSIGNED_BYTE, tex_data.data());
                    glyph.texture = texture;
                    glyph.uv = {(f32)(row.pen_pos) / (f32)font_texture_width, (f32)(row.y_start) / (f32)font_texture_height,
                                (f32)(row.pen_pos + bitmap_width) / (f32)font_texture_width, (f32)(row.y_start + bitmap_height) / (f32)font_texture_height};
                    // Flip uv because of OpenGL's flipped vertical axis.
                    // glyph.uv.top = 1.0f - glyph.uv.top;
                    // glyph.uv.bottom = 1.0f - glyph.uv.bottom;
                    row.pen_pos += bitmap_width + 1;
                    atlas->glyphs.emplace(codepoint, glyph);
                    return glyph;
                }
            }
        }

        // We haven't found a slot in existing rows.
        // Try to create new row in an existing texture.
        for(i64 i = 0; i < rows.size(); ++i) {
            auto& tex_rows = rows[i];
            Font_Row& last = tex_rows[tex_rows.size() - 1];
            i32 const available_height = font_texture_height - last.y_end;
            if(required_row_height <= available_height) {
                // Set pen_pos to bitmap_width + 1.
                Font_Row& row = tex_rows.emplace_back(last.y_end, last.y_end + required_row_height, bitmap_width + 1);
                // Copy bitmap to texture.
                u32 const texture = atlas->textures[i];
                glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
                glTextureSubImage2D(texture, 0, 0, row.y_start, bitmap_width, bitmap_height, GL_RED, GL_UNSIGNED_BYTE, tex_data.data());
                glyph.texture = texture;
                glyph.uv = {0.0f, (f32)row.y_start / (f32)font_texture_height, (f32)bitmap_width / (f32)font_texture_width,
                            (f32)(row.y_start + bitmap_height) / (f32)font_texture_height};
                // Flip uv because of OpenGL's flipped vertical axis.
                // glyph.uv.top = 1.0f - glyph.uv.top;
                // glyph.uv.bottom = 1.0f - glyph.uv.bottom;
                atlas->glyphs.emplace(codepoint, glyph);
                return glyph;
            }
        }

        // We couldn't create a new row in an existing texture.
        // Create new texture.
        u32 texture;
        glCreateTextures(GL_TEXTURE_2D, 1, &texture);
        glTextureStorage2D(texture, 1, GL_R8, font_texture_width, font_texture_height);
        i32 const swizzle[] = {GL_ONE, GL_ONE, GL_ONE, GL_RED};
        glTextureParameteriv(texture, GL_TEXTURE_SWIZZLE_RGBA, swizzle);
        glTextureParameteri(texture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTextureParameteri(texture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        f32 const border_color[] = {0.0f, 0.0f, 0.0f, 0.0f};
        glTextureParameterfv(texture, GL_TEXTURE_BORDER_COLOR, border_color);
        atlas->textures.emplace_back(texture);

        auto& new_rows = atlas->rows.emplace_back();
        // Set pen_pos to bitmap_width + 1.
        new_rows.emplace_back(0, required_row_height, bitmap_width + 1);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTextureSubImage2D(texture, 0, 0, 0, bitmap_width, bitmap_height, GL_RED, GL_UNSIGNED_BYTE, tex_data.data());

        glyph.texture = texture;
        glyph.uv = {0.0f, 0.0f, (f32)bitmap_width / (f32)font_texture_width, (f32)bitmap_height / (f32)font_texture_height};
        // Flip uv because of OpenGL's flipped vertical axis.
        // glyph.uv.top = 1.0f - glyph.uv.top;
        // glyph.uv.bottom = 1.0f - glyph.uv.bottom;
        atlas->glyphs.emplace(codepoint, glyph);
        return glyph;
    }

    atl::Vector<Glyph> render_text(Font_Face* const face, Font_Render_Info const info, atl::String_View const string) {
        i64 const text_size = unicode::convert_utf8_to_utf32(string.data(), string.size_bytes(), nullptr) / sizeof(char32);
        atl::Vector<char32> text_utf32{atl::reserve, text_size};
        text_utf32.force_size(text_size);
        unicode::convert_utf8_to_utf32(string.data(), string.size_bytes(), text_utf32.data());
        // TODO: Layout via HarfBuzz.
        atl::Vector<Glyph> glyphs(atl::reserve, text_size);
        for(char32 const c: text_utf32) {
            // TODO: Handle space, ignore newline
            // Omit all null-terminators because they rasterize to missing character (empty rectangle, etc).
            if(c != U'\0' && c != U'\n' && c != U' ') {
                Glyph const glyph = rasterize_glyph(*font_lib, face, info, c);
                glyphs.emplace_back(glyph);
            }
        }
        return glyphs;
    }
} // namespace anton_engine::rendering
