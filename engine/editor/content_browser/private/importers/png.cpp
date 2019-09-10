#include <importers/png.hpp>

#include <debug_macros.hpp> // ANTON_UNREACHABLE
#include <anton_stl/type_traits.hpp>
#include <anton_stl/vector.hpp>
#include <cstdint>
#include <importers/common.hpp>
#include <math/math.hpp>
#include <stdexcept>
#include <zlib.h>

namespace anton_engine::importers {
    constexpr uint64_t png_header = 0x89504E470D0A1A0A;

    // Chunk properties bits
    constexpr uint32_t ancillary_bit = 0x10000000;
    // constexpr uint32_t private_bit = 0x00100000; // Currently unused
    // constexpr uint32_t reserved_bit = 0x00001000; // Currently unused
    // constexpr uint32_t safe_to_copy_bit = 0x00000010; // Currently unused

    /* color types */
    constexpr uint8_t color_type_greyscale = 0;
    constexpr uint8_t color_type_truecolor = 2;
    constexpr uint8_t color_type_indexed = 3;
    constexpr uint8_t color_type_greyscale_alpha = 4;
    constexpr uint8_t color_type_truecolor_alpha = 6;

    // constexpr uint8_t color_type_indexed_bit = 1; // Currently unused
    constexpr uint8_t color_type_truecolor_bit = 2;
    constexpr uint8_t color_type_alpha_bit = 4;

    /* filter methods */
    constexpr uint8_t filter_none = 0;
    constexpr uint8_t filter_sub = 1;
    constexpr uint8_t filter_up = 2;
    constexpr uint8_t filter_average = 3;
    constexpr uint8_t filter_paeth = 4;

    /* Critical chunks */
    // Single IHDR chunk allowed
    constexpr uint32_t chunk_IHDR = 0x49484452;
    // Optional palette chunk before IDAT
    constexpr uint32_t chunk_PLTE = 0x504C5445;
    // IDAT chunks must appear in succession
    constexpr uint32_t chunk_IDAT = 0x49444154;
    // Single IEND at the end
    constexpr uint32_t chunk_IEND = 0x49454E44;
    /* Ancillary chunks */
    constexpr uint32_t chunk_cHRM = 0x6348524D;
    constexpr uint32_t chunk_gAMA = 0x67414D41;
    constexpr uint32_t chunk_iCCP = 0x69434350;
    constexpr uint32_t chunk_sBIT = 0x73424954;
    constexpr uint32_t chunk_sRBG = 0x73524247;
    constexpr uint32_t chunk_bKGD = 0x624B4744;
    constexpr uint32_t chunk_hIST = 0x68495354;
    constexpr uint32_t chunk_tRNS = 0x74524E53;
    constexpr uint32_t chunk_pHYs = 0x70485973;
    constexpr uint32_t chunk_sPLT = 0x73504C54;
    constexpr uint32_t chunk_tIME = 0x74494D45;
    constexpr uint32_t chunk_iTXt = 0x69545874;
    constexpr uint32_t chunk_tEXt = 0x74455874;
    constexpr uint32_t chunk_zTXt = 0x7A545874;

    static uint8_t paeth_predictor(int32_t const a, int32_t const b, int32_t const c) {
        int32_t const p = a + b - c;
        int32_t const pa = math::abs(p - a);
        int32_t const pb = math::abs(p - b);
        int32_t const pc = math::abs(p - c);
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
    static uint8_t reconstruct_sample(uint8_t const filter, uint8_t const x, uint8_t const a, uint8_t const b, uint8_t const c) {
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
    static void reconstruct_scanlines(uint8_t const* const pixels, int32_t const pixel_width, int64_t const scanline_width, uint32_t const scanlines_total,
                                      uint8_t* const out_pixels) {
        // Reconstruct first scanline
        {
            uint8_t const scanline_filter = pixels[0];
            // First pixel
            for (int64_t i = 0; i < pixel_width; ++i) {
                out_pixels[i] = reconstruct_sample(scanline_filter, pixels[i + 1], 0, 0, 0);
            }
            // Other pixels
            for (int64_t i = pixel_width; i < scanline_width; i += 1) {
                out_pixels[i] = reconstruct_sample(scanline_filter, pixels[i + 1], out_pixels[i - pixel_width], 0, 0);
            }
        }

        // Reconstruct remaining scanlines
        for (uint32_t scanline_index = 1; scanline_index < scanlines_total; ++scanline_index) {
            uint8_t const scanline_filter = pixels[scanline_index * (scanline_width + 1)];
            int64_t const prev_offset = scanline_width * (scanline_index - 1);
            int64_t const in_offset = (scanline_width + 1) * scanline_index;
            int64_t const out_offset = scanline_width * scanline_index;
            // First pixel
            for (int32_t i = 0; i < pixel_width; ++i) {
                out_pixels[out_offset + i] = reconstruct_sample(scanline_filter, pixels[in_offset + i + 1], 0, out_pixels[prev_offset + i], 0);
            }
            // Other pixels
            for (uint32_t i = pixel_width; i < scanline_width; i += 1) {
                uint8_t current_byte = pixels[in_offset + i + 1];
                uint8_t prev_byte = out_pixels[out_offset + i - pixel_width];
                uint8_t up_byte = out_pixels[prev_offset + i];
                uint8_t prev_up_byte = out_pixels[prev_offset + i - pixel_width];
                out_pixels[out_offset + i] = reconstruct_sample(scanline_filter, current_byte, prev_byte, up_byte, prev_up_byte);
            }
        }
    }

    static void extract_adam7_pass(int const pass, uint8_t const*& pixels, int32_t const pixel_width, uint64_t const image_width, uint64_t const image_height,
                                   anton_stl::Vector<uint8_t>& out_pixels) {
        uint32_t const starting_row[7] = {0, 0, 4, 0, 2, 0, 1};
        uint32_t const row_increment[7] = {8, 8, 8, 4, 4, 2, 2};
        uint32_t const starting_column[7] = {0, 4, 0, 2, 0, 1, 0};
        uint32_t const column_increment[7] = {8, 8, 4, 4, 2, 2, 1};
        for (uint64_t row = starting_row[pass]; row < image_height; row += row_increment[pass]) {
            for (uint64_t column = starting_column[pass]; column < image_width; column += column_increment[pass]) {
                for (int32_t i = 0; i < pixel_width; ++i, ++pixels) {
                    uint64_t position = (row * image_height + column) * pixel_width + i;
                    out_pixels[position] = *pixels;
                }
            }
        }
    }

    struct Image_Header {
        uint32_t width;
        uint32_t height;
        uint8_t bit_depth;
        uint8_t color_type;
        uint8_t compression_method;
        uint8_t filter_method;
        uint8_t interlace_method;
    };

    struct Chunk_Data {
        uint32_t data_length;
        uint32_t chunk_type;
        uint8_t const* data;
        uint32_t crc;
    };

    static uint8_t const* read_bytes(uint8_t const* const stream, uint64_t const byte_count, int64_t& pos) {
        uint64_t const pos_copy = pos;
        pos += byte_count;
        return stream + pos_copy;
    }

    static Chunk_Data read_chunk(uint8_t const* const stream, int64_t& pos) {
        uint32_t const data_length = read_uint32_be(stream, pos);
        uint32_t const chunk_type = read_uint32_be(stream, pos);
        uint8_t const* const data = read_bytes(stream, data_length, pos);
        uint32_t const crc = read_uint32_be(stream, pos);
        return {data_length, chunk_type, data, crc};
    }

    static uint32_t get_pixel_width(uint8_t const color_type, uint8_t const bit_depth) {
        uint32_t const bytes_per_sample = (bit_depth == 16 ? 2 : 1);
        uint32_t width = 0;
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

    static Image_Header reinterpret_bytes_as_image_header(uint8_t const* bytes) {
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

    static uint64_t get_decompression_buffer_size(int64_t const width, int64_t const height, int64_t const pixel_width, bool const interlaced) {
        if (!interlaced) {
            return width * height * pixel_width + height;
        } else {
            int64_t const row_increment[7] = {8, 8, 8, 4, 4, 2, 2};
            int64_t const column_increment[7] = {8, 8, 4, 4, 2, 2, 1};
            // Adam7 starting positions incremented by 1
            int64_t const starting_row[7] = {1, 1, 5, 1, 3, 1, 2};
            int64_t const starting_column[7] = {1, 5, 1, 3, 1, 2, 1};
            uint64_t size = 0;
            for (int i = 0; i < 7; ++i) {
                int64_t const scanlines = height < starting_row[i] ? 0 : (height - starting_row[i]) / row_increment[i] + 1;
                int64_t const pixels = width < starting_column[i] ? 0 : (width - starting_column[i]) / column_increment[i] + 1;
                size += (pixels * pixel_width) * scanlines + (pixels != 0 ? scanlines : 0);
            }
            return size;
        }
    }

    bool test_png(anton_stl::Vector<uint8_t> const& image_data) {
        uint64_t const header = read_uint64_be(image_data.data());
        return header == png_header;
    }

    Image import_png(anton_stl::Vector<uint8_t> const& png_data) {
        // TODO Reduce number of memory allocations
        // TODO Make sure less than 8 bit images are handled correctly
        // TODO Maybe filter/deinterlace on the fly?
        // TODO Add CRC checking
        int64_t stream_pos = 8; // Skip png header which is 8 bytes long
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

        uint64_t const pixel_width = get_pixel_width(header.color_type, header.bit_depth);
        uint64_t pixels_buffer_size = get_decompression_buffer_size(header.width, header.height, pixel_width, header.interlace_method);
        anton_stl::Vector<uint8_t> pixels(pixels_buffer_size);
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

        uint8_t palette_entries = 0;
        struct Indexed_Color {
            uint8_t red;
            uint8_t green;
            uint8_t blue;
        };
        anton_stl::Vector<Indexed_Color> color_palette(1 << 8);
        anton_stl::Vector<uint8_t> alpha_palette(1 << 8, 255);
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
                    if (palette_entries > (2 << math::min(header.bit_depth, static_cast<uint8_t>(8)))) {
                        throw Invalid_Image_File("PLTE chunk contains too many entries");
                    }

                    anton_stl::copy(chunk_data.data, chunk_data.data + chunk_data.data_length, reinterpret_cast<uint8_t*>(color_palette.data()));
                    color_palette.resize(palette_entries);
                    PLTE_present = true;
                    break;
                case chunk_IDAT: {
                    if (header.color_type == color_type_indexed && !PLTE_present) {
                        throw Invalid_Image_File("Missing PLTE chunk");
                    }

                    stream.next_in = const_cast<uint8_t*>(chunk_data.data);
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
                        uint32_t gamma_int = read_uint32_be(chunk_data.data);
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

                        anton_stl::copy(chunk_data.data, chunk_data.data + chunk_data.data_length, alpha_palette.data());
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

        anton_stl::Vector<uint8_t> pixels_unfiltered(header.height * header.width * pixel_width);
        if (header.interlace_method == 0) {
            uint64_t const scanline_width = header.width * pixel_width;
            reconstruct_scanlines(pixels.data(), pixel_width, scanline_width, header.height, pixels_unfiltered.data());
        } else {
            int64_t const row_increment[7] = {8, 8, 8, 4, 4, 2, 2};
            int64_t const column_increment[7] = {8, 8, 4, 4, 2, 2, 1};
            // Adam7 starting positions incremented by 1
            int64_t const starting_row[7] = {1, 1, 5, 1, 3, 1, 2};
            int64_t const starting_column[7] = {1, 5, 1, 3, 1, 2, 1};
            uint8_t const* in_pixels = pixels.data();
            uint8_t* out_pixels = pixels_unfiltered.data();
            int64_t const height = static_cast<int64_t>(header.height);
            int64_t const width = static_cast<int64_t>(header.width);
            for (int i = 0; i < 7; ++i) {
                if (height >= starting_row[i] && width >= starting_column[i]) {
                    int64_t const pass_scanline_total = (height - starting_row[i]) / row_increment[i] + 1;
                    // Width without the filter byte
                    int64_t const pass_scanline_width = ((width - starting_column[i]) / column_increment[i] + 1) * pixel_width;
                    reconstruct_scanlines(in_pixels, pixel_width, pass_scanline_width, pass_scanline_total, out_pixels);
                    in_pixels += pass_scanline_width * pass_scanline_total + pass_scanline_total;
                    out_pixels += pass_scanline_width * pass_scanline_total;
                }
            }
        }

        if (header.interlace_method == 1) {
            pixels.resize(header.width * header.height * pixel_width);
            uint8_t const* pixels_ptr = pixels_unfiltered.data();
            for (int i = 0; i < 7; ++i) {
                extract_adam7_pass(i, pixels_ptr, pixel_width, header.width, header.height, pixels);
            }
            pixels_unfiltered = std::move(pixels);
        }

        if (header.color_type == color_type_indexed) {
            uint64_t indexed_pixel_width = tRNS_present ? 4 : 3;
            anton_stl::Vector<uint8_t> deindexed(anton_stl::reserve, header.width * header.height * indexed_pixel_width);
            if (tRNS_present) {
                for (uint8_t pixel_index: pixels_unfiltered) {
                    auto color = color_palette[pixel_index];
                    deindexed.push_back(color.red);
                    deindexed.push_back(color.green);
                    deindexed.push_back(color.blue);
                    deindexed.push_back(alpha_palette[pixel_index]);
                }
            } else {
                for (uint8_t pixel_index: pixels_unfiltered) {
                    auto color = color_palette[pixel_index];
                    deindexed.push_back(color.red);
                    deindexed.push_back(color.green);
                    deindexed.push_back(color.blue);
                }
            }
            pixels_unfiltered = std::move(deindexed);
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
        return {header.width, header.height, pixel_format, Image_Color_Space::gamma_encoded, gamma, anton_stl::move(pixels_unfiltered)};
    }
} // namespace anton_engine::importers
