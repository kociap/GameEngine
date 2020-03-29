#include <content_browser/importers/common.hpp>

namespace anton_engine::importers {
    u8 read_uint8(u8 const* const stream, i64& pos) {
        pos += 1;
        return *(stream + pos - 1);
    }

    // Little-endian read functions

    u16 read_uint16_le(u8 const* const stream) {
        return static_cast<u16>(stream[0]) | (static_cast<u16>(stream[1]) << 8);
    }

    u32 read_uint32_le(u8 const* const stream) {
        return static_cast<u32>(stream[0]) | (static_cast<u32>(stream[1]) << 8) | (static_cast<u32>(stream[2]) << 16) |
               (static_cast<u32>(stream[3]) << 24);
    }

    u64 read_uint64_le(u8 const* const stream) {
        return static_cast<u64>(stream[0]) | (static_cast<u64>(stream[1]) << 8) | (static_cast<u64>(stream[2]) << 16) |
               (static_cast<u64>(stream[3]) << 24) | (static_cast<u64>(stream[4]) << 32) | (static_cast<u64>(stream[5]) << 40) |
               (static_cast<u64>(stream[6]) << 48) | (static_cast<u64>(stream[7]) << 56);
    }

    u16 read_uint16_le(u8 const* const stream, i64& pos) {
        pos += 2;
        return read_uint16_le(stream + pos - 2);
    }

    u32 read_uint32_le(u8 const* const stream, i64& pos) {
        pos += 4;
        return read_uint32_le(stream + pos - 4);
    }

    u64 read_uint64_le(u8 const* const stream, i64& pos) {
        pos += 8;
        return read_uint64_le(stream + pos - 8);
    }

    // Big endian read functions

    u16 read_uint16_be(u8 const* const stream) {
        return (static_cast<u16>(stream[0]) << 8) | static_cast<u16>(stream[1]);
    }

    u32 read_uint32_be(u8 const* const stream) {
        return (static_cast<u32>(stream[0]) << 24) | (static_cast<u32>(stream[1]) << 16) | (static_cast<u32>(stream[2]) << 8) |
               static_cast<u32>(stream[3]);
    }

    u64 read_uint64_be(u8 const* const stream) {
        return (static_cast<u64>(stream[0]) << 56) | (static_cast<u64>(stream[1]) << 48) | (static_cast<u64>(stream[2]) << 40) |
               (static_cast<u64>(stream[3]) << 32) | (static_cast<u64>(stream[4]) << 24) | (static_cast<u64>(stream[5]) << 16) |
               (static_cast<u64>(stream[6]) << 8) | static_cast<u64>(stream[7]);
    }

    u16 read_uint16_be(u8 const* const stream, i64& pos) {
        pos += 2;
        return read_uint16_be(stream + pos - 2);
    }

    u32 read_uint32_be(u8 const* const stream, i64& pos) {
        pos += 4;
        return read_uint32_be(stream + pos - 4);
    }

    u64 read_uint64_be(u8 const* const stream, i64& pos) {
        pos += 8;
        return read_uint64_be(stream + pos - 8);
    }
} // namespace anton_engine::importers
