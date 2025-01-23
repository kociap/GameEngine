#include <content_browser/importers/tga.hpp>

#include <anton/memory.hpp>
#include <content_browser/importers/common.hpp>
#include <core/types.hpp>

namespace anton_engine::importers {
  constexpr i32 footer_byte_size = 26;
  constexpr i32 extension_area_size = 495;
  // Offsets of fields in the extension area relative to its start
  // constexpr u32 extension_author_name_offset = 2; // Currently unused
  // constexpr u32 extension_author_comments_offset = 43; // Currently unused
  // constexpr u32 extension_date_offset = 367; // Currently unused
  // constexpr u32 extension_job_name_offset = 379; // Currently unused
  // constexpr u32 extension_job_time_offset = 420; // Currently unused
  // constexpr u32 extension_software_id_offset = 426; // Currently unused
  // constexpr u32 extension_software_version_offset = 467; // Currently unused
  // constexpr u32 extension_key_color_offset = 470; // Currently unused
  // constexpr u32 extension_pixel_aspect_ratio_offset = 474; // Currently unused
  constexpr i32 extension_gamma_value_offset = 478;
  // constexpr u32 extension_color_correction_offset_offset = 482; // Currently unused
  // constexpr u32 extension_postage_stamp_offset_offset = 486; // Currently unused
  // constexpr u32 extension_scan_line_offset_offset = 490; // Currently unused
  // constexpr u32 extension_attributes_type_offset = 494; // Currently unused

  // The signature in the TGA footer
  constexpr u64 tga_signature_0_8 = 0x5452554556495349;
  constexpr u64 tga_signature_8_16 = 0x4F4E2D5846494C45;

  constexpr u8 img_type_no_image_data = 0;
  constexpr u8 img_type_uncompressed_color_mapped = 1;
  constexpr u8 img_type_uncompressed_true_color = 2;
  constexpr u8 img_type_uncompressed_black_white = 3;
  // Run-length encoded
  constexpr u8 img_type_RLE_color_mapped = 9;
  constexpr u8 img_type_RLE_true_color = 10;
  constexpr u8 img_type_RLE_black_white = 11;

  struct TGA_Header {
    i32 image_width;
    i32 image_height;
    u16 first_entry_index;
    u16 color_map_length;
    u16 image_x_origin;
    u16 image_y_origin;
    u8 id_length;
    u8 color_map_type;
    u8 image_type;
    u8 color_map_entry_size;
    u8 pixel_depth;
    u8 image_descriptor;
  };

  static TGA_Header read_header(u8 const* const stream, i64& pos)
  {
    // Image width and height are 2 byte unsigned integers
    TGA_Header header;
    header.id_length = read_uint8(stream, pos);
    header.color_map_type = read_uint8(stream, pos);
    header.image_type = read_uint8(stream, pos);
    header.first_entry_index = read_uint16_le(stream, pos);
    header.color_map_length = read_uint16_le(stream, pos);
    header.color_map_entry_size = read_uint8(stream, pos);
    header.image_x_origin = read_uint16_le(stream, pos);
    header.image_y_origin = read_uint16_le(stream, pos);
    header.image_width = static_cast<i32>(read_uint16_le(stream, pos));
    header.image_height = static_cast<i32>(read_uint16_le(stream, pos));
    header.pixel_depth = read_uint8(stream, pos);
    header.image_descriptor = read_uint8(stream, pos);
    return header;
  }

  struct TGA_Footer {
    u32 extension_area_offset;
    u32 developer_area_offset;
    u64 signature1;
    u64 signature2;
    u8 reserved_character;
    u8 terminator;
  };

  static TGA_Footer read_footer(u8 const* const stream, i64& pos)
  {
    TGA_Footer footer;
    footer.extension_area_offset = read_uint32_le(stream, pos);
    footer.developer_area_offset = read_uint32_le(stream, pos);
    // Reinterpret tga signature as two numbers
    footer.signature1 = read_uint64_be(stream, pos);
    footer.signature2 = read_uint64_be(stream, pos);
    footer.reserved_character = read_uint8(stream, pos);
    footer.terminator = read_uint8(stream, pos);
    return footer;
  }

  // TGA2 test
  bool test_tga(anton::Array<u8> const& stream)
  {
    i64 pos =
      stream.size() -
      footer_byte_size; // Seek to the end of the file to look for the footer
    TGA_Footer const footer = read_footer(stream.data(), pos);
    return footer.signature1 == tga_signature_0_8 &&
           footer.signature2 == tga_signature_8_16 &&
           footer.reserved_character == '.' && footer.terminator == '\0';
  }

  Image import_tga(anton::Array<u8> const& tga_data)
  {
    u8 const* const tga_stream = tga_data.data();
    i64 pos = 0;
    TGA_Header const header = read_header(tga_stream, pos);

    if(header.pixel_depth != 8 && header.pixel_depth != 16 &&
       header.pixel_depth != 24 && header.pixel_depth != 32) {
      // TODO maybe support 15 bit pixel depth
      throw Invalid_Image_File("Unsupported pixel depth");
    }

    if(header.image_type == img_type_no_image_data) {
      throw Invalid_Image_File("File contains no image data");
    }

    u8 const* image_id = nullptr;
    if(header.id_length != 0) {
      image_id = tga_stream + pos;
      pos += header.id_length;
    }

    u8 const* color_map = nullptr;
    if(header.color_map_type != 0) {
      color_map = tga_stream + pos;
      pos += header.color_map_length;
    }

    i64 const bytes_per_pixel = static_cast<i64>(header.pixel_depth / 8);
    i64 const image_data_length = static_cast<i64>(header.image_width) *
                                  static_cast<i64>(header.image_height) *
                                  bytes_per_pixel;
    u8 const* const image_data = tga_stream + pos;

    Image image;
    image.width = header.image_width;
    image.height = header.image_height;
    image.data.resize(image_data_length);

    bool const is_greyscale =
      header.image_type == img_type_RLE_black_white ||
      header.image_type == img_type_uncompressed_black_white;
    bool const is_indexed =
      header.image_type == img_type_RLE_color_mapped ||
      header.image_type == img_type_uncompressed_color_mapped;
    bool const is_truecolor =
      header.image_type == img_type_RLE_true_color ||
      header.image_type == img_type_uncompressed_true_color;
    // Width of the pixel in the final decoded image
    u8 const pixel_width =
      (!is_indexed ? header.pixel_depth : header.color_map_entry_size);
    // Number of bits in the alpha channel
    u8 const alpha_bits = header.image_descriptor & 0x0F;
    switch(pixel_width) {
    case 8:
      if(is_greyscale && alpha_bits == 0) {
        image.pixel_format = Image_Pixel_Format::grey8;
      } else {
        throw Invalid_Image_File("Unknown pixel format");
      }
      break;
    case 16:
      // TODO add rgb5_a1
      if(is_greyscale && alpha_bits == 8) {
        image.pixel_format = Image_Pixel_Format::grey8_alpha8;
      } else {
        throw Invalid_Image_File("Unknown pixel format");
      }
      break;
    case 24:
      if(is_truecolor && alpha_bits == 0) {
        image.pixel_format = Image_Pixel_Format::rgb8;
      } else {
        throw Invalid_Image_File("Unknown pixel format");
      }
      break;
    case 32:
      if(is_truecolor && alpha_bits == 8) {
        image.pixel_format = Image_Pixel_Format::rgba8;
      } else {
        throw Invalid_Image_File("Unknown pixel format");
      }
      break;
    default:
      ANTON_UNREACHABLE();
    }

    // Swap bytes of pixels and copy them to out
    auto swap_and_copy_bytes =
      [alpha_bits](u8* const out, u8 const* const in, i64 const bytes_to_copy,
                   i64 const bytes_per_chunk, bool const indexed) {
        for(i64 offset = 0; offset < bytes_to_copy; offset += bytes_per_chunk) {
          switch(bytes_per_chunk) {
          case 1:
            out[offset] = in[offset];
            break;
          case 2:
            if(alpha_bits == 8) {
              // Just copy because the color is stored as RA in little-endian and we want RA format
              out[offset] = in[offset];
              out[offset + 1] = in[offset + 1];
            } else {
              out[offset] = in[offset + 1];
              out[offset + 1] = in[offset];
            }
            break;
          case 3:
            out[offset] = in[offset + 2];
            out[offset + 1] = in[offset + 1];
            out[offset + 2] = in[offset];
            break;
          case 4:
            if(!indexed) {
              // We want to get the color as RGBA, so we only swap RGB bytes and leave alpha byte unmodified
              out[offset] = in[offset + 2];
              out[offset + 1] = in[offset + 1];
              out[offset + 2] = in[offset];
              out[offset + 3] = in[offset + 3];
            } else {
              // Swap all bytes because it's an int32
              out[offset] = in[offset + 3];
              out[offset + 1] = in[offset + 2];
              out[offset + 2] = in[offset + 1];
              out[offset + 3] = in[offset];
            }
            break;
          default:
            ANTON_UNREACHABLE();
          }
        }
      };

    // We cannot simply copy the data from the file to our storage because tga is the dumbest format ever
    //   where non-related bytes are stored in little-endian, a.k.a. RGB is BGR.
    // We have to perform a lot of byte swapping, bit shifting and what not.
    if(header.image_type == img_type_RLE_black_white ||
       header.image_type == img_type_RLE_color_mapped ||
       header.image_type == img_type_RLE_true_color) {
      for(i64 offset = 0, img_offset = 0; img_offset < image_data_length;) {
        u8 const is_RL_packet = *(image_data + offset) & 0x80;
        u8 const repeat_count = (*(image_data + offset) & 0x7F) + 1;
        u8 const* const current_pixel = image_data + offset + 1;
        if(is_RL_packet) {
          u8* const swapped_pixel = image.data.data() + img_offset;
          swap_and_copy_bytes(swapped_pixel, current_pixel, bytes_per_pixel,
                              bytes_per_pixel, is_indexed);
          img_offset += bytes_per_pixel;
          for(i32 j = 1; j < repeat_count; ++j) {
            anton::copy(swapped_pixel, swapped_pixel + bytes_per_pixel,
                        image.data.data() + img_offset);
            img_offset += bytes_per_pixel;
          }
          offset += bytes_per_pixel + 1;
        } else {
          swap_and_copy_bytes(image.data.data() + img_offset, current_pixel,
                              repeat_count * bytes_per_pixel, bytes_per_pixel,
                              is_indexed);
          img_offset += repeat_count * bytes_per_pixel;
          offset += repeat_count * bytes_per_pixel + 1;
        }
      }
    } else {
      swap_and_copy_bytes(image.data.data(), image_data, image_data_length,
                          bytes_per_pixel, is_indexed);
    }

    if(is_indexed) {
      // Deindexing
      anton::Array<u8> colors(header.image_width * header.image_height *
                              pixel_width);
      i32 const bytes_per_color_map_pixel = static_cast<i32>(pixel_width / 8);
      u8* indices_ptr = image.data.data();
      u8* const colors_ptr = colors.data();
      for(i64 offset = 0; offset < image.data.size();) {
        u64 index = 0;
        // Read as big-endian because we did byte-swapping
        switch(bytes_per_color_map_pixel) {
        case 1:
          index = *indices_ptr;
          break;
        case 2:
          index = read_uint16_be(indices_ptr);
          break;
        case 3:
          index = (static_cast<u64>(read_uint16_be(indices_ptr)) << 8) +
                  indices_ptr[2];
          break;
        case 4:
          index = read_uint32_be(indices_ptr);
          break;
        }

        swap_and_copy_bytes(
          colors_ptr + offset, color_map + index * pixel_width,
          bytes_per_color_map_pixel, bytes_per_color_map_pixel, false);
        offset += bytes_per_color_map_pixel;
        indices_ptr += bytes_per_pixel;
      }
      image.data = ANTON_MOV(colors);
    }

    i64 footer_pos = tga_data.size() - footer_byte_size;
    TGA_Footer const footer = read_footer(tga_stream, footer_pos);
    u16 gamma_numerator = 0;
    u16 gamma_denominator = 0;
    if(footer.extension_area_offset != 0) {
      i32 extension_size = static_cast<i32>(
        read_uint16_le(tga_stream + footer.extension_area_offset));
      if(extension_size != extension_area_size) {
        throw Invalid_Image_File(
          "TGA extension size is not 495. Unknown TGA format");
      }

      gamma_numerator =
        read_uint16_le(tga_stream + footer.extension_area_offset +
                       extension_gamma_value_offset);
      gamma_denominator =
        read_uint16_le(tga_stream + footer.extension_area_offset +
                       extension_gamma_value_offset + 2);
    }

    if(gamma_denominator != 0) {
      image.gamma = static_cast<float>(gamma_numerator) /
                    static_cast<float>(gamma_denominator);
      if(image.gamma == 2.2f) {
        // Let's say that gamma of 2.2 means sRGB...
        image.color_space = Image_Color_Space::srgb;
      } else if(image.gamma == 1.0f) {
        // ...and that gamma of 1.0 means linear space
        image.color_space = Image_Color_Space::linear;
      } else {
        image.color_space = Image_Color_Space::gamma_encoded;
      }
    } else {
      // Gamma information not provided. Assume sRGB
      image.gamma = 2.2f;
      image.color_space = Image_Color_Space::srgb;
    }

    return image;
  }
} // namespace anton_engine::importers
