#include <content_browser/importers/png.hpp>

#include <core/intrinsics.hpp>
#include <core/atl/type_traits.hpp>
#include <core/atl/vector.hpp>
#include <core/types.hpp>
#include <content_browser/importers/common.hpp>
#include <core/math/math.hpp>
#include <stdexcept>
#include <zlib.h>

namespace anton_engine::importers {
    constexpr u64 png_header = 0x89504E470D0A1A0A;

    // Chunk properties bits
    constexpr u32 ancillary_bit = 0x10000000;
    // constexpr u32 private_bit = 0x00100000; // Currently unused
    // constexpr u32 reserved_bit = 0x00001000; // Currently unused
    // constexpr u32 safe_to_copy_bit = 0x00000010; // Currently unused

    /* color types */
    constexpr u8 color_type_greyscale = 0;
    constexpr u8 color_type_truecolor = 2;
    constexpr u8 color_type_indexed = 3;
    constexpr u8 color_type_greyscale_alpha = 4;
    constexpr u8 color_type_truecolor_alpha = 6;

    // constexpr u8 color_type_indexed_bit = 1; // Currently unused
    constexpr u8 color_type_truecolor_bit = 2;
    constexpr u8 color_type_alpha_bit = 4;

    /* filter methods */
    constexpr u8 filter_none = 0;
    constexpr u8 filter_sub = 1;
    constexpr u8 filter_up = 2;
    constexpr u8 filter_average = 3;
    constexpr u8 filter_paeth = 4;

    /* Critical chunks */
    // Single IHDR chunk allowed
    constexpr u32 chunk_IHDR = 0x49484452;
    // Optional palette chunk before IDAT
    constexpr u32 chunk_PLTE = 0x504C5445;
    // IDAT chunks must appear in succession
    constexpr u32 chunk_IDAT = 0x49444154;
    // Single IEND at the end
    constexpr u32 chunk_IEND = 0x49454E44;
    /* Ancillary chunks */
    constexpr u32 chunk_cHRM = 0x6348524D;
    constexpr u32 chunk_gAMA = 0x67414D41;
    constexpr u32 chunk_iCCP = 0x69434350;
    constexpr u32 chunk_sBIT = 0x73424954;
    constexpr u32 chunk_sRBG = 0x73524247;
    constexpr u32 chunk_bKGD = 0x624B4744;
    constexpr u32 chunk_hIST = 0x68495354;
    constexpr u32 chunk_tRNS = 0x74524E53;
    constexpr u32 chunk_pHYs = 0x70485973;
    constexpr u32 chunk_sPLT = 0x73504C54;
    constexpr u32 chunk_tIME = 0x74494D45;
    constexpr u32 chunk_iTXt = 0x69545874;
    constexpr u32 chunk_tEXt = 0x74455874;
    constexpr u32 chunk_zTXt = 0x7A545874;

    static u8 paeth_predictor(i32 const a, i32 const b, i32 const c) {
        i32 const p = a + b - c;
        i32 const pa = math::abs(p - a);
        i32 const pb = math::abs(p - b);
        i32 const pc = math::abs(p - c);
        if (pa <= pb && pa <= pc) {
            return a;
        } else if (pb <= pc) {
            return b;
        } else {
            return c;
        }
    }

    // x - byte being processed
    // a - byte corresponding to x in the pixel before
    // b - byte corresponding to x in the previous scanline
    // c - byte corresponding to a in the previous scanline
    static u8 reconstruct_sample(u8 const filter, u8 const x, u8 const a, u8 const b, u8 const c) {
        switch (filter) {
            case filter_none:
                return x;
            case filter_sub:
                return x + a;
            case filter_up:
                return x + b;
            case filter_average:
                return x + ((a + b) >> 1);
            case filter_paeth:
                return x + paeth_predictor(a, b, c);
            default:
                throw Invalid_Image_File("Invalid filter type");
        }
    }

    // pixel_width - number of bytes per pixel
    // scanline_width - number of bytes without the filter byte per scanline
    // scanlines_total - number of scanlines
    static void reconstruct_scanlines(u8 const* const pixels, i32 const pixel_width, i64 const scanline_width, u32 const scanlines_total,
                                      u8* const out_pixels) {
        // Reconstruct first scanline
        {
            u8 const scanline_filter = pixels[0];
            // First pixel
            for (i64 i = 0; i < pixel_width; ++i) {
                out_pixels[i] = reconstruct_sample(scanline_filter, pixels[i + 1], 0, 0, 0);
            }
            // Other pixels
            for (i64 i = pixel_width; i < scanline_width; i += 1) {
                out_pixels[i] = reconstruct_sample(scanline_filter, pixels[i + 1], out_pixels[i - pixel_width], 0, 0);
            }
        }

        // Reconstruct remaining scanlines
        for (u32 scanline_index = 1; scanline_index < scanlines_total; ++scanline_index) {
            u8 const scanline_filter = pixels[scanline_index * (scanline_width + 1)];
            i64 const prev_offset = scanline_width * (scanline_index - 1);
            i64 const in_offset = (scanline_width + 1) * scanline_index;
            i64 const out_offset = scanline_width * scanline_index;
            // First pixel
            for (i32 i = 0; i < pixel_width; ++i) {
                out_pixels[out_offset + i] = reconstruct_sample(scanline_filter, pixels[in_offset + i + 1], 0, out_pixels[prev_offset + i], 0);
            }
            // Other pixels
            for (u32 i = pixel_width; i < scanline_width; i += 1) {
                u8 current_byte = pixels[in_offset + i + 1];
                u8 prev_byte = out_pixels[out_offset + i - pixel_width];
                u8 up_byte = out_pixels[prev_offset + i];
                u8 prev_up_byte = out_pixels[prev_offset + i - pixel_width];
                out_pixels[out_offset + i] = reconstruct_sample(scanline_filter, current_byte, prev_byte, up_byte, prev_up_byte);
            }
        }
    }

    static void extract_adam7_pass(int const pass, u8 const*& pixels, i32 const pixel_width, u64 const image_width, u64 const image_height,
                                   atl::Vector<u8>& out_pixels) {
        u32 const starting_row[7] = {0, 0, 4, 0, 2, 0, 1};
        u32 const row_increment[7] = {8, 8, 8, 4, 4, 2, 2};
        u32 const starting_column[7] = {0, 4, 0, 2, 0, 1, 0};
        u32 const column_increment[7] = {8, 8, 4, 4, 2, 2, 1};
        for (u64 row = starting_row[pass]; row < image_height; row += row_increment[pass]) {
            for (u64 column = starting_column[pass]; column < image_width; column += column_increment[pass]) {
                for (i32 i = 0; i < pixel_width; ++i, ++pixels) {
                    u64 position = (row * image_height + column) * pixel_width + i;
                    out_pixels[position] = *pixels;
                }
            }
        }
    }

    struct Image_Header {
        u32 width;
        u32 height;
        u8 bit_depth;
        u8 color_type;
        u8 compression_method;
        u8 filter_method;
        u8 interlace_method;
    };

    struct Chunk_Data {
        u32 data_length;
        u32 chunk_type;
        u8 const* data;
        u32 crc;
    };

    static u8 const* read_bytes(u8 const* const stream, u64 const byte_count, i64& pos) {
        u64 const pos_copy = pos;
        pos += byte_count;
        return stream + pos_copy;
    }

    static Chunk_Data read_chunk(u8 const* const stream, i64& pos) {
        u32 const data_length = read_uint32_be(stream, pos);
        u32 const chunk_type = read_uint32_be(stream, pos);
        u8 const* const data = read_bytes(stream, data_length, pos);
        u32 const crc = read_uint32_be(stream, pos);
        return {data_length, chunk_type, data, crc};
    }

    static u32 get_pixel_width(u8 const color_type, u8 const bit_depth) {
        u32 const bytes_per_sample = (bit_depth == 16 ? 2 : 1);
        u32 width = 0;
        if (color_type == color_type_indexed) {
            return 1; // always 1 byte per sample which is an index into the palette
        }

        if (color_type & color_type_truecolor_bit) {
            width = 3 * bytes_per_sample;
        } else {
            // greyscale, single sample
            width = bytes_per_sample;
        }

        if (color_type & color_type_alpha_bit) {
            width += bytes_per_sample;
        }

        return width;
    }

    static Image_Header reinterpret_bytes_as_image_header(u8 const* bytes) {
        Image_Header header;
        header.width = read_uint32_be(bytes);
        header.height = read_uint32_be(bytes + 4);
        header.bit_depth = *(bytes + 8);
        header.color_type = *(bytes + 8 + 1);
        header.compression_method = *(bytes + 8 + 2);
        header.filter_method = *(bytes + 8 + 3);
        header.interlace_method = *(bytes + 8 + 4);
        return header;
    }

    static u64 get_decompression_buffer_size(i64 const width, i64 const height, i64 const pixel_width, bool const interlaced) {
        if (!interlaced) {
            return width * height * pixel_width + height;
        } else {
            i64 const row_increment[7] = {8, 8, 8, 4, 4, 2, 2};
            i64 const column_increment[7] = {8, 8, 4, 4, 2, 2, 1};
            // Adam7 starting positions incremented by 1
            i64 const starting_row[7] = {1, 1, 5, 1, 3, 1, 2};
            i64 const starting_column[7] = {1, 5, 1, 3, 1, 2, 1};
            u64 size = 0;
            for (int i = 0; i < 7; ++i) {
                i64 const scanlines = height < starting_row[i] ? 0 : (height - starting_row[i]) / row_increment[i] + 1;
                i64 const pixels = width < starting_column[i] ? 0 : (width - starting_column[i]) / column_increment[i] + 1;
                size += (pixels * pixel_width) * scanlines + (pixels != 0 ? scanlines : 0);
            }
            return size;
        }
    }

    bool test_png(atl::Vector<u8> const& image_data) {
        u64 const header = read_uint64_be(image_data.data());
        return header == png_header;
    }

    Image import_png(atl::Vector<u8> const& png_data) {
        // TODO Reduce number of memory allocations
        // TODO Make sure less than 8 bit images are handled correctly
        // TODO Maybe filter/deinterlace on the fly?
        // TODO Add CRC checking
        i64 stream_pos = 8; // Skip png header which is 8 bytes long
        Chunk_Data const header_data = read_chunk(png_data.data(), stream_pos);
        if (header_data.chunk_type != chunk_IHDR) {
            throw Invalid_Image_File("IHDR chunk is not first");
        }

        if (header_data.data_length != 13) {
            throw Invalid_Image_File("IHDR chunk length is not 13");
        }

        Image_Header const header = reinterpret_bytes_as_image_header(header_data.data);
        if (header.width == 0 || header.height == 0) {
            throw Invalid_Image_File("Width or height of the image may not be 0");
        }

        auto ct = header.color_type;
        if (ct != 0 && ct != 2 && ct != 3 && ct != 4 && ct != 6) {
            throw Invalid_Image_File("Invalid color type");
        }

        auto bd = header.bit_depth;
        if (bd != 1 && bd != 2 && bd != 4 && bd != 8 && bd != 16) {
            throw Invalid_Image_File("Invalid bith depth");
        }

        bool valid_color_type_bit_depth_combination =
            ((bd == 1 || bd == 2 || bd == 4) && (ct == 0 || ct == 3)) || ((bd == 8 || bd == 16) && (ct == 0 || ct == 2 || ct == 3 || ct == 4 || ct == 6));
        if (!valid_color_type_bit_depth_combination) {
            throw Invalid_Image_File("Invalid color type bit depth combination");
        }

        if (header.compression_method != 0) {
            throw Invalid_Image_File("Invalid compression method");
        }

        if (header.filter_method != 0) {
            throw Invalid_Image_File("Invalid filter method");
        }

        if (header.interlace_method != 0 && header.interlace_method != 1) {
            throw Invalid_Image_File("Invalid interlace method");
        }

        u64 const pixel_width = get_pixel_width(header.color_type, header.bit_depth);
        u64 pixels_buffer_size = get_decompression_buffer_size(header.width, header.height, pixel_width, header.interlace_method);
        atl::Vector<u8> pixels(pixels_buffer_size);
        z_stream stream;
        stream.next_out = pixels.data();
        stream.avail_out = pixels.size();
        stream.next_in = nullptr;
        stream.avail_in = 0;
        stream.zalloc = nullptr;
        stream.zfree = nullptr;
        stream.opaque = nullptr;
        if (auto init_successful = inflateInit(&stream); init_successful != Z_OK) {
            throw Decompression_Failure("Failed to initialize the decompression engine");
        }

        u8 palette_entries = 0;
        struct Indexed_Color {
            u8 red;
            u8 green;
            u8 blue;
        };
        atl::Vector<Indexed_Color> color_palette(1 << 8);
        atl::Vector<u8> alpha_palette(1 << 8, 255);
        bool tRNS_present = false;
        float gamma = 2.2f;
        Image_Color_Space color_space = Image_Color_Space::srgb;
        for (bool end_loop = false, iCCP_present = false, sRGB_present = false, PLTE_present = false, IDAT_read = false; !end_loop;) {
            Chunk_Data chunk_data = read_chunk(png_data.data(), stream_pos);
            switch (chunk_data.chunk_type) {
                // Critical chunks
                case chunk_IHDR:
                    throw Invalid_Image_File("Multiple IHDR chunks not allowed");
                case chunk_PLTE:
                    if (PLTE_present) {
                        throw Invalid_Image_File("Multiple PLTE chunks not allowed");
                    }

                    if (chunk_data.data_length % 3 != 0) {
                        throw Invalid_Image_File("PLTE chunk length not divisible by 3");
                    }

                    palette_entries = chunk_data.data_length / 3;
                    if (palette_entries > (2 << math::min(header.bit_depth, static_cast<u8>(8)))) {
                        throw Invalid_Image_File("PLTE chunk contains too many entries");
                    }

                    atl::copy(chunk_data.data, chunk_data.data + chunk_data.data_length, reinterpret_cast<u8*>(color_palette.data()));
                    color_palette.resize(palette_entries);
                    PLTE_present = true;
                    break;
                case chunk_IDAT: {
                    if (header.color_type == color_type_indexed && !PLTE_present) {
                        throw Invalid_Image_File("Missing PLTE chunk");
                    }

                    stream.next_in = const_cast<u8*>(chunk_data.data);
                    stream.avail_in = chunk_data.data_length;
                    auto inflate_res = inflate(&stream, Z_NO_FLUSH);
                    if (inflate_res != Z_OK && inflate_res != Z_STREAM_END) {
                        throw Decompression_Failure("Could not finish decompression");
                    }
                    if (inflate_res == Z_STREAM_END) {
                        IDAT_read = true;
                    }

                    break;
                }
                case chunk_IEND:
                    if (!IDAT_read) {
                        throw Invalid_Image_File("Missing IDAT chunks");
                    }
                    inflateEnd(&stream);
                    end_loop = true;
                    break;

                // Ancillary chunks
                case chunk_cHRM:
                    break;
                case chunk_gAMA:
                    if (!iCCP_present && !sRGB_present) {
                        u32 gamma_int = read_uint32_be(chunk_data.data);
                        if (gamma_int == 45455U) {
                            color_space = Image_Color_Space::srgb;
                            gamma = 2.2f;
                        } else {
                            color_space = Image_Color_Space::gamma_encoded;
                            gamma = 100000.0f / static_cast<float>(gamma_int);
                        }
                    }
                    break;
                case chunk_iCCP:
                    if (sRGB_present) {
                        throw Invalid_Image_File("sRGB and iCCP chunks are both present, which is not allowed");
                    }
                    iCCP_present = true;
                    break;
                case chunk_sBIT:
                    break;
                case chunk_sRBG:
                    if (iCCP_present) {
                        throw Invalid_Image_File("sRGB and iCCP chunks are both present, which is not allowed");
                    }
                    color_space = Image_Color_Space::srgb;
                    // sRGB requires a custom conversion function, but we set gamma to 2.2 because it
                    //   is considered a good approximation
                    gamma = 2.2f;
                    sRGB_present = true;
                    break;
                case chunk_bKGD:
                    break;
                case chunk_hIST:
                    break;
                case chunk_tRNS:
                    if (tRNS_present) {
                        throw Invalid_Image_File("Multiple tRNS chunks not allowed");
                    }

                    if (header.color_type == color_type_greyscale_alpha || header.color_type == color_type_truecolor_alpha) {
                        throw Invalid_Image_File("tRNS chunk forbidden when color type has an alpha channel");
                    }

                    // "Ignore" the tRNS chunk if the color type is truecolor or greyscale.
                    // Proper handling of those would significantly increase the complexity of the code
                    //   without providing many benefits since they are rather uncommon
                    if (header.color_type == color_type_indexed) {
                        if (chunk_data.data_length > color_palette.size()) {
                            throw Invalid_Image_File("Too many entries in the alpha palette");
                        }

                        atl::copy(chunk_data.data, chunk_data.data + chunk_data.data_length, alpha_palette.data());
                        alpha_palette.resize(color_palette.size());
                    }
                    tRNS_present = true;
                    break;
                case chunk_pHYs:
                    break;
                case chunk_sPLT:
                    break;

                // Ancillary metadata
                case chunk_tIME:
                    break;
                case chunk_iTXt:
                    break;
                case chunk_tEXt:
                    break;
                case chunk_zTXt:
                    break;

                default:
                    // An unknown chunk. If it's not critical, ignore it
                    if (!(chunk_data.chunk_type & ancillary_bit)) {
                        throw Unknown_Critical_Chunk();
                    }
            }
        }

        atl::Vector<u8> pixels_unfiltered(header.height * header.width * pixel_width);
        if (header.interlace_method == 0) {
            u64 const scanline_width = header.width * pixel_width;
            reconstruct_scanlines(pixels.data(), pixel_width, scanline_width, header.height, pixels_unfiltered.data());
        } else {
            i64 const row_increment[7] = {8, 8, 8, 4, 4, 2, 2};
            i64 const column_increment[7] = {8, 8, 4, 4, 2, 2, 1};
            // Adam7 starting positions incremented by 1
            i64 const starting_row[7] = {1, 1, 5, 1, 3, 1, 2};
            i64 const starting_column[7] = {1, 5, 1, 3, 1, 2, 1};
            u8 const* in_pixels = pixels.data();
            u8* out_pixels = pixels_unfiltered.data();
            i64 const height = static_cast<i64>(header.height);
            i64 const width = static_cast<i64>(header.width);
            for (int i = 0; i < 7; ++i) {
                if (height >= starting_row[i] && width >= starting_column[i]) {
                    i64 const pass_scanline_total = (height - starting_row[i]) / row_increment[i] + 1;
                    // Width without the filter byte
                    i64 const pass_scanline_width = ((width - starting_column[i]) / column_increment[i] + 1) * pixel_width;
                    reconstruct_scanlines(in_pixels, pixel_width, pass_scanline_width, pass_scanline_total, out_pixels);
                    in_pixels += pass_scanline_width * pass_scanline_total + pass_scanline_total;
                    out_pixels += pass_scanline_width * pass_scanline_total;
                }
            }
        }

        if (header.interlace_method == 1) {
            pixels.resize(header.width * header.height * pixel_width);
            u8 const* pixels_ptr = pixels_unfiltered.data();
            for (int i = 0; i < 7; ++i) {
                extract_adam7_pass(i, pixels_ptr, pixel_width, header.width, header.height, pixels);
            }
            pixels_unfiltered = atl::move(pixels);
        }

        if (header.color_type == color_type_indexed) {
            u64 indexed_pixel_width = tRNS_present ? 4 : 3;
            atl::Vector<u8> deindexed(atl::reserve, header.width * header.height * indexed_pixel_width);
            if (tRNS_present) {
                for (u8 pixel_index: pixels_unfiltered) {
                    auto color = color_palette[pixel_index];
                    deindexed.push_back(color.red);
                    deindexed.push_back(color.green);
                    deindexed.push_back(color.blue);
                    deindexed.push_back(alpha_palette[pixel_index]);
                }
            } else {
                for (u8 pixel_index: pixels_unfiltered) {
                    auto color = color_palette[pixel_index];
                    deindexed.push_back(color.red);
                    deindexed.push_back(color.green);
                    deindexed.push_back(color.blue);
                }
            }
            pixels_unfiltered = atl::move(deindexed);
            // pixel_width = indexed_pixel_width; // Not required since we don't use the pixel width after deindexing
        }

        Image_Pixel_Format pixel_format;
        switch (header.color_type) {
            case color_type_greyscale:
                if (header.bit_depth != 16) {
                    pixel_format = Image_Pixel_Format::grey8;
                } else {
                    pixel_format = Image_Pixel_Format::grey16;
                }
                break;
            case color_type_truecolor:
                if (header.bit_depth != 16) {
                    pixel_format = Image_Pixel_Format::rgb8;
                } else {
                    pixel_format = Image_Pixel_Format::rgb16;
                }
                break;
            case color_type_indexed:
                if (tRNS_present) {
                    pixel_format = Image_Pixel_Format::rgba8;
                } else {
                    pixel_format = Image_Pixel_Format::rgb8;
                }
                break;
            case color_type_greyscale_alpha:
                if (header.bit_depth != 16) {
                    pixel_format = Image_Pixel_Format::grey8_alpha8;
                } else {
                    pixel_format = Image_Pixel_Format::grey16_alpha16;
                }
                break;
            case color_type_truecolor_alpha:
                if (header.bit_depth != 16) {
                    pixel_format = Image_Pixel_Format::rgba8;
                } else {
                    pixel_format = Image_Pixel_Format::rgba16;
                }
                break;
            default:
                ANTON_UNREACHABLE();
        }
        // TODO: Choose color space based on the image loaded
        return {header.width, header.height, pixel_format, Image_Color_Space::gamma_encoded, gamma, atl::move(pixels_unfiltered)};
    }
} // namespace anton_engine::importers
