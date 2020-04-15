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

    Face_Metrics get_face_metrics(Font_Face* _face) {
        FT_Face face = reinterpret_cast<FT_Face>(_face);
        Face_Metrics metrics;
        metrics.family_name = face->family_name;
        metrics.style_name = face->style_name;
        metrics.ascent = face->ascender;
        metrics.descent = face->descender;
        metrics.line_height = face->height;
        metrics.max_advance = face->max_advance_width;
        metrics.units_per_em = face->units_per_EM;
        return metrics;
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
            if(buffer_utf32[i] == U'\0') {
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
        
        i64 const buf_size = unicode::convert_utf8_to_utf32(string.data(), string.size_bytes(), nullptr) / sizeof(char32);
        atl::Vector<char32> buffer_utf32{atl::reserve, buf_size};
        buffer_utf32.force_size(buf_size);
        unicode::convert_utf8_to_utf32(string.data(), string.size_bytes(), buffer_utf32.data());
        i64 width = 0;
        i64 height_above_baseline = 0;
        i64 height_below_baseline = 0;

        i64 i = 0;
        // Skip all null-terminators.
        for(; buffer_utf32[i] == U'\n' && i < buf_size; ++i);

        // Special handling for first character.
        if(i < buf_size) {
            u32 const glyph_index = FT_Get_Char_Index(face, buffer_utf32[i]);
            if(FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT)) {
                throw Exception(u8"Could not compute text size: failed to load the glyph.");
            }

            FT_Glyph_Metrics& metrics = face->glyph->metrics;
            height_above_baseline = metrics.horiBearingY;
            height_below_baseline = metrics.height - metrics.horiBearingY;
            if(i + 1 != buf_size) {
                if(metrics.horiBearingX >= 0) {
                    width += metrics.horiAdvance;
                } else {
                    // Handle the case of negative left side bearing
                    width += -metrics.horiBearingX + metrics.horiAdvance;
                }
            } else {
                // Handle the case of this being the only glyph.
                width = metrics.width;
            }
            ++i;
        }

        for(; i < buf_size; ++i) {
            // Skip null-terminator because it's non-printable, but rasterizes to rectangle.
            if(buffer_utf32[i] == U'\0') {
                continue;
            }

            u32 const glyph_index = FT_Get_Char_Index(face, buffer_utf32[i]);
            if(FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT)) {
                throw Exception(u8"Could not compute text size: failed to load the glyph.");
            }

            FT_Glyph_Metrics& metrics = face->glyph->metrics;
            height_above_baseline = math::max(height_above_baseline, (i64)(metrics.horiBearingY));
            height_below_baseline = math::max(height_below_baseline, (i64)((metrics.height - metrics.horiBearingY)));
            if(i + 1 != buf_size) {
                width += metrics.horiAdvance;
            } else {
                width += metrics.horiBearingX + metrics.width;
            }
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
        if(glyphs.size() == 0) {
            return {};
        }

        i64 text_pixel_width = 0;
        i64 height_above_baseline = 0;
        i64 height_below_baseline = 0;

        // Special handling of first glyph due to possibly negative left side bearing.
        {
            Glyph const& glyph = glyphs[0];
            height_above_baseline = glyph.hori_bearing_y;
            height_below_baseline = glyph.height - glyph.hori_bearing_y;
            if(glyphs.size() != 1) {
                if(glyph.hori_bearing_x >= 0) {
                    text_pixel_width += glyph.hori_advance;
                } else {
                    // Handle the less frequent case of negative left side bearing
                    text_pixel_width += -glyph.hori_bearing_x + glyph.hori_advance;
                }
            } else {
                // If it's also the last glyph, add whole bitmap row size because advance might be smaller than glyph size and 
                // therefore the buffer wouldn't be large enough to fit the whole text.
                text_pixel_width = glyph.bitmap_row_width * 64;
            }
        }

        for(i64 i = 1; i < glyphs.size(); i += 1) {
            Glyph const& glyph = glyphs[i];
            // If it's last glyph, add whole bitmap row size because advance might be smaller than glyph size and 
            // therefore the buffer wouldn't be large enough to fit the whole text.
            height_above_baseline = math::max(height_above_baseline, (i64)glyph.hori_bearing_y);
            height_below_baseline = math::max(height_below_baseline, (i64)(glyph.height - glyph.hori_bearing_y));
            if(i + 1 < glyphs.size()) {
                text_pixel_width += glyph.hori_advance;
            } else {
                text_pixel_width += glyph.bitmap_row_width * 64;
            }
        }

        i64 const buffer_height = (height_above_baseline + height_below_baseline) / 64 + !!((height_above_baseline + height_below_baseline) & 0x3F);
        i64 const buffer_width = text_pixel_width / 64 + !!(text_pixel_width & 0x3F);
        atl::Vector<u8> tex_data_f32{buffer_width * buffer_height, 0};
        // If first glyph's left bearing is negative, move pen position so we don't write before the buffer.
        i64 pen_position = (glyphs[0].hori_bearing_x >= 0 ? 0 : -glyphs[0].hori_bearing_x);
        for(Glyph const& glyph: glyphs) {
            // Spaces and other non-printable characters do not produce glyph representations.
            // The only non-zero field on those glyphs is advance width.
            if(glyph.bitmap_row_width == 0) {
                pen_position += glyph.hori_advance;
                continue;
            }

            i64 const glyph_position = pen_position + glyph.hori_bearing_x;
            i64 const vertical_offset = height_above_baseline - glyph.hori_bearing_y;
            i64 const bitmap_height = glyph.bitmap.size() / glyph.bitmap_row_width;
            // In case when the text has no vertical subpixel offset and a glyph is the height of the buffer we would write past the end.
            // We workaround that by adding special case to handle perfectly aligned glyphs.
            if(vertical_offset & 0x3F) {
                // Has vertical subpixel offset.
                if(glyph_position & 0x3F) {
                    // Has horizontal subpixel offset.
                    f32 const fract_y = (f32)(vertical_offset & 0x3F) / 64.0f;
                    f32 const one_fract_y = 1.0f - fract_y;
                    f32 const fract_x = (f32)(glyph_position & 0x3F) / 64.0f;
                    f32 const one_fract_x = 1.0f - fract_x;
                    for(i64 i = 0; i < bitmap_height; i += 1) {
                        i64 const offset = buffer_width * (i + vertical_offset / 64) + glyph_position / 64;
                        i64 const next_row_offset = buffer_width * (i + vertical_offset / 64 + 1) + glyph_position / 64;
                        for(i64 j = 0; j < glyph.bitmap_row_width; j += 1) {
                            // Linearly distribute bitmap pixel value over the 4 overlapped pixels.
                            f32 const bitmap_pixel = glyph.bitmap[i * glyph.bitmap_row_width + j];
                            tex_data_f32[offset + j] = math::min(tex_data_f32[offset + j] + bitmap_pixel * one_fract_x * one_fract_y, 255.0f);
                            tex_data_f32[offset + j + 1] = math::min(tex_data_f32[offset + j + 1] + bitmap_pixel * fract_x * one_fract_y, 255.0f);
                            tex_data_f32[next_row_offset + j] = math::min(tex_data_f32[next_row_offset + j] + bitmap_pixel * one_fract_x * fract_y, 255.0f);
                            tex_data_f32[next_row_offset + j + 1] = math::min(tex_data_f32[next_row_offset + j + 1] + bitmap_pixel * fract_x * fract_y, 255.0f);
                        }
                    }
                } else {
                    f32 const fract_y = (f32)(vertical_offset & 0x3F) / 64.0f;
                    f32 const one_fract_y = 1.0f - fract_y;
                    for(i64 i = 0; i < bitmap_height; i += 1) {
                        i64 const offset = buffer_width * (i + vertical_offset / 64) + glyph_position / 64;
                        i64 const next_row_offset = buffer_width * (i + vertical_offset / 64 + 1) + glyph_position / 64;
                        for(i64 j = 0; j < glyph.bitmap_row_width; j += 1) {
                            // Overlaps 2 vertical pixels. Distribute the bitmap pixel value linearily over them.
                            f32 const bitmap_pixel = glyph.bitmap[i * glyph.bitmap_row_width + j];
                            tex_data_f32[offset + j] = math::min(tex_data_f32[offset + j] + bitmap_pixel * one_fract_y, 255.0f);
                            tex_data_f32[next_row_offset + j] = math::min(tex_data_f32[next_row_offset + j] + bitmap_pixel * fract_y, 255.0f);
                        }
                    }
                }
            } else {
                if(glyph_position & 0x3F) {
                    // Has horizontal subpixel offset.
                    f32 const fract_x = (f32)(glyph_position & 0x3F) / 64.0f;
                    f32 const one_fract_x = 1.0f - fract_x;
                    for(i64 i = 0; i < bitmap_height; i += 1) {
                        i64 const offset = buffer_width * (i + vertical_offset / 64) + glyph_position / 64;
                        for(i64 j = 0; j < glyph.bitmap_row_width; j += 1) {
                            // Overlaps 2 horizontal pixels. Distribute the bitmap pixel value linearily over them.
                            f32 const bitmap_pixel = glyph.bitmap[i * glyph.bitmap_row_width + j];
                            tex_data_f32[offset + j] = math::min(tex_data_f32[offset + j] + bitmap_pixel * one_fract_x, 255.0f);
                            tex_data_f32[offset + j + 1] = math::min(tex_data_f32[offset + j + 1] + bitmap_pixel * fract_x, 255.0f);
                        }
                    }
                } else {
                    for(i64 i = 0; i < bitmap_height; i += 1) {
                        i64 const offset = buffer_width * (i + vertical_offset / 64) + glyph_position / 64;
                        for(i64 j = 0; j < glyph.bitmap_row_width; j += 1) {
                            // Perfectly aligned. Just add.
                            f32 const bitmap_pixel = glyph.bitmap[i * glyph.bitmap_row_width + j];
                            tex_data_f32[offset + j] = math::min(tex_data_f32[offset + j] + bitmap_pixel, 255.0f);
                        }
                    }
                }
            }
            pen_position += glyph.hori_advance;
        }

        atl::Vector<u8> tex_data{atl::range_construct, tex_data_f32.begin(), tex_data_f32.end()};
        flip_texture(tex_data, buffer_width, buffer_height);

        Text_Image image;
        image.baseline = height_above_baseline;
        image.width = buffer_width * 64;
        image.height = buffer_height * 64;
        glCreateTextures(GL_TEXTURE_2D, 1, &image.texture);
        glTextureStorage2D(image.texture, 1, GL_R8, buffer_width, buffer_height);
        i32 const swizzle[] = {GL_ONE, GL_ONE, GL_ONE, GL_RED};
        glTextureParameteriv(image.texture, GL_TEXTURE_SWIZZLE_RGBA, swizzle);
        glTextureParameteri(image.texture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTextureParameteri(image.texture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        f32 const border_color[] = {0.0f, 0.0f, 0.0f, 0.0f};
        glTextureParameterfv(image.texture, GL_TEXTURE_BORDER_COLOR, border_color);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTextureSubImage2D(image.texture, 0, 0, 0, buffer_width, buffer_height, GL_RED, GL_UNSIGNED_BYTE, tex_data.data());
        atl::Vector<Text_Image_With_Parameters>& textures = font_lib->font_texture_map.find_or_emplace(str_hash)->value;
        textures.emplace_back(_face, info, image);
        return image;
    }
} // namespace anton_engine::rendering
