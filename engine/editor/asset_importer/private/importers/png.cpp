#include <containers/vector.hpp>
#include <cstdint>
#include <importers/png.hpp>
#include <iostream>
#include <math/math.hpp>
#include <stdexcept>
#include <zlib.h>

namespace importers {
    constexpr uint64_t png_header = 0x89504E470D0A1A0A;

    // Chunk properties bits
    constexpr uint32_t ancillary_bit = 0x10000000;
    constexpr uint32_t private_bit = 0x00100000;
    constexpr uint32_t reserved_bit = 0x00001000;
    constexpr uint32_t safe_to_copy_bit = 0x00000010;

    /* color types */
    constexpr uint8_t color_type_greyscale = 0;
    constexpr uint8_t color_type_truecolor = 2;
    constexpr uint8_t color_type_indexed = 3;
    constexpr uint8_t color_type_greyscale_alpha = 4;
    constexpr uint8_t color_type_truecolor_alpha = 6;

    constexpr uint8_t color_type_indexed_bit = 1;
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
        int32_t p = a + b - c;
        int32_t pa = math::abs(p - a);
        int32_t pb = math::abs(p - b);
        int32_t pc = math::abs(p - c);
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

    // Read 16bit big-endian unsigned integer
    static uint16_t read_uint16_be(uint8_t const* const stream) {
        return (static_cast<uint16_t>(*stream) << 8) | static_cast<uint16_t>(*(stream + sizeof(uint8_t)));
    }

    // Read 32bit big-endian unsigned integer
    static uint32_t read_uint32_be(uint8_t const* const stream) {
        return (static_cast<uint32_t>(read_uint16_be(stream)) << 16) | static_cast<uint32_t>(read_uint16_be(stream + sizeof(uint16_t)));
    }

    // Read 64bit big-endian unsigned integer
    static uint64_t read_uint64_be(uint8_t const* const stream) {
        return (static_cast<uint64_t>(read_uint32_be(stream)) << 32) | static_cast<uint64_t>(read_uint32_be(stream + sizeof(uint32_t)));
    }

    static uint32_t read_uint32_be(uint8_t const* const stream, uint64_t& pos) {
        uint32_t val = (static_cast<uint32_t>(*(stream + pos)) << 24) | (static_cast<uint32_t>(*(stream + pos + 1)) << 16) |
                       (static_cast<uint32_t>(*(stream + pos + 2)) << 8) | static_cast<uint32_t>(*(stream + pos + 3));
        pos += sizeof(uint32_t);
        return val;
    }

    static uint8_t const* read_bytes(uint8_t const* const stream, uint64_t const byte_count, uint64_t& pos) {
        uint64_t const pos_copy = pos;
        pos += byte_count;
        return stream + pos_copy;
    }

    static Chunk_Data read_chunk(uint8_t const* const stream, uint64_t& pos) {
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
        uint64_t stream_pos = 0;
        header.width = read_uint32_be(bytes, stream_pos);
        header.height = read_uint32_be(bytes, stream_pos);
        header.bit_depth = *(bytes + stream_pos);
        header.color_type = *(bytes + stream_pos + 1);
        header.compression_method = *(bytes + stream_pos + 2);
        header.filter_method = *(bytes + stream_pos + 3);
        header.interlace_method = *(bytes + stream_pos + 4);
        return header;
    }

    bool test_png(containers::Vector<uint8_t> const& image_data) {
        uint64_t header = read_uint64_be(image_data.data());
        return header == png_header;
    }

    Image_Data import_png(containers::Vector<uint8_t> const& png_data) {
        uint64_t stream_pos = 8; // Skip png header which is 8 bytes long
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

        uint64_t pixel_width = get_pixel_width(header.color_type, header.bit_depth);
        containers::Vector<uint8_t> pixels(static_cast<uint64_t>(header.height) * static_cast<uint64_t>(header.width) * pixel_width + header.height);
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
        containers::Vector<Indexed_Color> color_palette(1 << 8);
        containers::Vector<uint8_t> alpha_palette(1 << 8, 255);
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

                    memory::copy(chunk_data.data, chunk_data.data + chunk_data.data_length, reinterpret_cast<uint8_t*>(color_palette.data()));
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

                        memory::copy(chunk_data.data, chunk_data.data + chunk_data.data_length, alpha_palette.data());
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

        containers::Vector<uint8_t> pixels_unfiltered(header.height * header.width * pixel_width, containers::reserve);
        uint64_t const scanline_width = header.width * pixel_width;
        uint64_t const scanline_with_filter_width = header.width * pixel_width + 1;

        auto get_filtered_byte = [scanline_width = scanline_with_filter_width, &pixels](uint64_t scanline_index, uint64_t pos) {
            return pixels[scanline_width * scanline_index + pos + 1];
        };

        auto get_reconstructed_byte = [scanline_width, &pixels = pixels_unfiltered](uint64_t scanline_index, uint64_t pos) {
            uint64_t index = scanline_index * scanline_width + pos;
            return pixels[index];
        };

        // Reconstruct first scanline
        {
            uint8_t const scanline_filter = pixels[0];
            // First pixel
            for (uint32_t i = 0; i < pixel_width; ++i) {
                pixels_unfiltered.push_back(reconstruct_sample(scanline_filter, get_filtered_byte(0, i), 0, 0, 0));
            }
            // Other pixels
            for (uint32_t i = pixel_width; i < scanline_width; i += 1) {
                pixels_unfiltered.push_back(reconstruct_sample(scanline_filter, get_filtered_byte(0, i), get_reconstructed_byte(0, i - pixel_width), 0, 0));
            }
        }

        // Reconstruct remaining scanlines
        for (uint32_t scanline_index = 1; scanline_index < header.height; ++scanline_index) {
            uint8_t const scanline_filter = pixels[scanline_index * scanline_with_filter_width];
            // First pixel
            for (uint32_t i = 0; i < pixel_width; ++i) {
                pixels_unfiltered.push_back(
                    reconstruct_sample(scanline_filter, get_filtered_byte(scanline_index, i), 0, get_reconstructed_byte(scanline_index - 1, i), 0));
            }
            // Other pixels
            for (uint64_t i = pixel_width; i < scanline_width; i += 1) {
                uint8_t current_byte = get_filtered_byte(scanline_index, i);
                uint8_t prev_byte = get_reconstructed_byte(scanline_index, i - pixel_width);
                uint8_t up_byte = get_reconstructed_byte(scanline_index - 1, i);
                uint8_t prev_up_byte = get_reconstructed_byte(scanline_index - 1, i - pixel_width);
                uint8_t reconstructed_byte = reconstruct_sample(scanline_filter, current_byte, prev_byte, up_byte, prev_up_byte);
                pixels_unfiltered.push_back(reconstructed_byte);
            }
        }

        if (header.color_type == color_type_indexed) {
            uint64_t indexed_pixel_width = tRNS_present ? 4 : 3;
            containers::Vector<uint8_t> deindexed(header.width * header.height * indexed_pixel_width, containers::reserve);
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
            pixel_width = indexed_pixel_width;
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
        }
        // TODO choose color space based on the image loaded
        return {header.width, header.height, pixel_format, Image_Color_Space::gamma_encoded, gamma, std::move(pixels_unfiltered)};
    }
} // namespace importers