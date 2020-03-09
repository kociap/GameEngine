#include <rendering/fonts.hpp>

#include <core/exception.hpp>
#include <core/diagnostic_macros.hpp>
#include <core/unicode/common.hpp>

ANTON_DISABLE_WARNINGS()
#include <ft2build.h>
#include FT_FREETYPE_H
ANTON_RESTORE_WARNINGS()

namespace anton_engine::rendering {
    static FT_Library ft_lib = nullptr;

    void init_font_rendering() {
        FT_Error const error = FT_Init_FreeType(&ft_lib);
        if(error) {
            throw Exception(u8"Could not initialize font rendering");
        }
    }

    void terminate_font_rendering() {
        FT_Done_FreeType(ft_lib);
        ft_lib = nullptr;
    }

    Font_Face* load_face(anton_stl::Slice<u8 const> const file_data, u32 const face_index) {
        FT_Face face;
        FT_Error const error = FT_New_Memory_Face(ft_lib, file_data.data(), file_data.size(), face_index, &face);
        if(error) {
            return nullptr;
        }

        return reinterpret_cast<Font_Face*>(face);
    }

    void unload_face(Font_Face* face) {
        FT_Done_Face(reinterpret_cast<FT_Face>(face));
    }

    anton_stl::Vector<Glyph> render_face(Font_Face* _face, Font_Render_Info const info, anton_stl::String_View const string) {
        FT_Face face = reinterpret_cast<FT_Face>(_face);

        if(FT_Set_Char_Size(face, 0, info.points * 64, info.h_dpi, info.v_dpi)) {
            throw Exception(u8"Could not render glyph: failed to set glyph size.");
        }
        
        // TODO: layout
        // TODO: caching
        
        i32 const buf_size = unicode::convert_utf8_to_utf32(string.data(), nullptr);
        anton_stl::Vector<char32> buffer_utf32{anton_stl::reserve, buf_size};
        buffer_utf32.force_size(buf_size);
        unicode::convert_utf8_to_utf32(string.data(), buffer_utf32.data());
        anton_stl::Vector<Glyph> glyphs;
        for(i32 i = 0; i < buf_size; ++i) {
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
            glyphs.emplace_back(metrics.width, metrics.height, metrics.horiBearingX, metrics.horiBearingY, metrics.horiAdvance, metrics.vertBearingX, metrics.vertBearingY, metrics.vertAdvance, (u32)math::abs(bitmap.pitch), anton_stl::Vector<u8>{anton_stl::range_construct, bitmap_buffer_begin, bitmap_buffer_end});
        }

        return glyphs;
    }
}
