#include <rendering/fonts.hpp>

#include <core/exception.hpp>
#include <core/diagnostic_macros.hpp>
#include <core/unicode/common.hpp>
#include <core/math/math.hpp>
#include <core/atl/flat_hash_map.hpp>

#include <rendering/glad.hpp>
#include <rendering/opengl.hpp>

ANTON_DISABLE_WARNINGS()
#include <ft2build.h>
#include FT_FREETYPE_H
ANTON_RESTORE_WARNINGS()

namespace anton_engine::rendering {
    struct Text_Image_With_Parameters {
        Font_Face* face;
        Font_Render_Info render_info;
        Text_Image image;
    };

    class Font_Library {
    public:
        atl::Flat_Hash_Map<u64, atl::Vector<Text_Image_With_Parameters>> font_texture_map;
    };

    static Font_Library* font_lib = nullptr;
    static FT_Library ft_lib = nullptr;

    void init_font_rendering() {
        FT_Error const error = FT_Init_FreeType(&ft_lib);
        if(error) {
            throw Exception(u8"Could not initialize font rendering");
        }

        font_lib = new Font_Library;
    }

    void terminate_font_rendering() {
        for(auto [key, value]: font_lib->font_texture_map) {
            for(Text_Image_With_Parameters& tex: value) {
                glDeleteTextures(1, &tex.image.texture);
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
        // TODO: Unload all rasterized glyphs.
        FT_Done_Face(reinterpret_cast<FT_Face>(face));
    }

    atl::Vector<Glyph> rasterize_text_glyphs(Font_Face* const _face, Font_Render_Info const info, atl::String_View const string) {
        FT_Face face = reinterpret_cast<FT_Face>(_face);

        if(FT_Set_Char_Size(face, 0, info.points * 64, info.h_dpi, info.v_dpi)) {
            throw Exception(u8"Could not render glyph: failed to set glyph size.");
        }
        
        // TODO: layout
        // TODO: caching
        
        i32 const buf_size = unicode::convert_utf8_to_utf32(string.data(), string.size_bytes(), nullptr) / sizeof(char32);
        atl::Vector<char32> buffer_utf32{atl::reserve, buf_size};
        buffer_utf32.force_size(buf_size);
        unicode::convert_utf8_to_utf32(string.data(), string.size_bytes(), buffer_utf32.data());
        atl::Vector<Glyph> glyphs;
        for(i32 i = 0; i < buf_size; ++i) {
            // Skip null-terminator because it's non-printable, but rasterizes to rectangle.
            if(buffer_utf32[i] == 0) {
                continue;
            }

            u32 const glyph_index = FT_Get_Char_Index(face, buffer_utf32[i]);
            if(FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT)) {
                throw Exception(u8"Could not render glyph: failed to load the glyph.");
            }

            if(FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL)) {
                throw Exception(u8"Could not render glyph.");
            }

            FT_Bitmap& bitmap = face->glyph->bitmap;
            FT_Glyph_Metrics& metrics = face->glyph->metrics;
            unsigned char* bitmap_buffer_begin = (bitmap.pitch > 0 ? bitmap.buffer : bitmap.buffer + bitmap.pitch * bitmap.rows);
            unsigned char* bitmap_buffer_end = (bitmap.pitch > 0 ? bitmap.buffer + bitmap.pitch * bitmap.rows : bitmap.buffer);
            glyphs.emplace_back(metrics.width, metrics.height, metrics.horiBearingX, metrics.horiBearingY, metrics.horiAdvance, /* metrics.vertBearingX, metrics.vertBearingY, metrics.vertAdvance, */ (u32)math::abs(bitmap.pitch), atl::Vector<u8>{atl::range_construct, bitmap_buffer_begin, bitmap_buffer_end});
        }

        return glyphs;
    }

    Text_Metrics compute_text_dimensions(Font_Face* const _face, Font_Render_Info const info, atl::String_View const string) {
        FT_Face face = reinterpret_cast<FT_Face>(_face);

        if(FT_Set_Char_Size(face, 0, info.points * 64, info.h_dpi, info.v_dpi)) {
            throw Exception(u8"Could not compute text size: failed to set glyph size.");
        }
        
        // TODO: layout
        // TODO: caching
        
        i32 const buf_size = unicode::convert_utf8_to_utf32(string.data(), string.size_bytes(), nullptr) / sizeof(char32);
        atl::Vector<char32> buffer_utf32{atl::reserve, buf_size};
        buffer_utf32.force_size(buf_size);
        unicode::convert_utf8_to_utf32(string.data(), string.size_bytes(), buffer_utf32.data());
        i32 width = 0;
        i32 height_above_baseline = 0;
        i32 height_below_baseline = 0;
        for(i32 i = 0; i < buf_size; ++i) {
            // Skip null-terminator because it's non-printable, but rasterizes to rectangle.
            if(buffer_utf32[i] == 0) {
                continue;
            }

            u32 const glyph_index = FT_Get_Char_Index(face, buffer_utf32[i]);
            if(FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT)) {
                throw Exception(u8"Could not compute text size: failed to load the glyph.");
            }

            FT_Glyph_Metrics& metrics = face->glyph->metrics;
            width += metrics.horiAdvance / 64;
            height_above_baseline = math::max(height_above_baseline, (i32)(metrics.horiBearingY / 64));
            height_below_baseline = math::max(height_below_baseline, (i32)((metrics.height - metrics.horiBearingY) / 64));
        }

        return Text_Metrics{ width, height_above_baseline + height_below_baseline, height_above_baseline };
    }

    static void flip_texture(atl::Slice<u8> tex, i64 const width, i64 const height) {
        i64 const rows_half = height / 2;
        for(i64 i = 0; i < rows_half; ++i) {
            i64 bottom_row_off = i * width;
            i64 top_row_off = (height - i - 1) * width;
            for(i64 j = 0; j < width; ++j) {
                atl::swap(tex[bottom_row_off + j], tex[top_row_off + j]);
            }
        }
    }

    Text_Image render_text(Font_Face* const _face, Font_Render_Info const info, atl::String_View const string) {
        u64 const str_hash = atl::hash(string);
        auto iter = font_lib->font_texture_map.find(str_hash);
        if(iter != font_lib->font_texture_map.end()) {
            atl::Vector<Text_Image_With_Parameters>& vec = iter->value;
            for(Text_Image_With_Parameters& tex: vec) {
                if(tex.face == _face && tex.render_info.points == info.points && 
                   tex.render_info.h_dpi == info.h_dpi && tex.render_info.v_dpi == info.v_dpi) {
                    return tex.image;
                }
            }
        }

        atl::Vector<Glyph> const glyphs = rasterize_text_glyphs(_face, info, string);
        i64 buffer_width = 0;
        i64 height_above_baseline = 0;
        i64 height_below_baseline = 0;
        i64 bitmap_width = 0;
        for(i64 i = 0; i < glyphs.size(); i += 1) {
            Glyph const& glyph = glyphs[i];
            bitmap_width += glyph.bitmap_row_width;
            // If it's last glyph, add whole bitmap row size because advance might be smaller than glyph size and 
            // therefore the buffer wouldn't be large enough to fit the whole text.
            buffer_width += (i + 1 < glyphs.size() ? glyph.hori_advance / 64 : glyph.bitmap_row_width);
            height_above_baseline = math::max(height_above_baseline, static_cast<i64>(glyph.hori_bearing_y / 64));
            height_below_baseline = math::max(height_below_baseline, static_cast<i64>((glyph.height - glyph.hori_bearing_y) / 64));
        }

        i64 buffer_height = height_above_baseline + height_below_baseline;
        atl::Vector<u8> tex_data{buffer_width * buffer_height, 0};
        i64 pen_position = 0;
        for(Glyph const& glyph: glyphs) {
            // Spaces and other non-printable characters do not produce glyph representations.
            // The only non-zero field on those glyphs is advance width.
            if(glyph.bitmap_row_width == 0) {
                pen_position += glyph.hori_advance / 64;
                continue;
            }

            i64 const vert_offset = height_above_baseline - glyph.hori_bearing_y / 64;
            i64 const bitmap_height = glyph.bitmap.size() / glyph.bitmap_row_width;
            for(i64 i = 0; i < bitmap_height; i += 1) {
                i64 const offset = buffer_width * (i + vert_offset) + pen_position;
                for(i64 j = 0; j < glyph.bitmap_row_width; j += 1) {
                    tex_data[offset + j] = math::min((u64)tex_data[offset + j] + (u64)glyph.bitmap[i * glyph.bitmap_row_width + j], (u64)255);
                }
            }
            pen_position += glyph.hori_advance / 64;
        }

        flip_texture(tex_data, buffer_width, buffer_height);

        Text_Image image;
        image.baseline = height_above_baseline;
        image.width = buffer_width;
        image.height = buffer_height;
        glCreateTextures(GL_TEXTURE_2D, 1, &image.texture);
        glTextureStorage2D(image.texture, 1, GL_R8, buffer_width, buffer_height);
        i32 const swizzle[] = {GL_ONE, GL_ONE, GL_ONE, GL_RED};
        glTextureParameteriv(image.texture, GL_TEXTURE_SWIZZLE_RGBA, swizzle);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTextureSubImage2D(image.texture, 0, 0, 0, buffer_width, buffer_height, GL_RED, GL_UNSIGNED_BYTE, tex_data.data());
        atl::Vector<Text_Image_With_Parameters>& textures = font_lib->font_texture_map.find_or_emplace(str_hash)->value;
        textures.emplace_back(_face, info, image);
        return image;
    }
} // namespace anton_engine::rendering
