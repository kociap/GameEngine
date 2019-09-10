#include <importers/common.hpp>

namespace anton_engine::importers {
    uint8_t read_uint8(uint8_t const* const stream, int64_t& pos) {
        pos += 1;
        return *(stream + pos - 1);
    }

    // Little-endian read functions

    uint16_t read_uint16_le(uint8_t const* const stream) {
        return static_cast<uint16_t>(stream[0]) | (static_cast<uint16_t>(stream[1]) << 8);
    }

    uint32_t read_uint32_le(uint8_t const* const stream) {
        return static_cast<uint32_t>(stream[0]) | (static_cast<uint32_t>(stream[1]) << 8) | (static_cast<uint32_t>(stream[2]) << 16) |
               (static_cast<uint32_t>(stream[3]) << 24);
    }

    uint64_t read_uint64_le(uint8_t const* const stream) {
        return static_cast<uint64_t>(stream[0]) | (static_cast<uint64_t>(stream[1]) << 8) | (static_cast<uint64_t>(stream[2]) << 16) |
               (static_cast<uint64_t>(stream[3]) << 24) | (static_cast<uint64_t>(stream[4]) << 32) | (static_cast<uint64_t>(stream[5]) << 40) |
               (static_cast<uint64_t>(stream[6]) << 48) | (static_cast<uint64_t>(stream[7]) << 56);
    }

    uint16_t read_uint16_le(uint8_t const* const stream, int64_t& pos) {
        pos += 2;
        return read_uint16_le(stream + pos - 2);
    }

    uint32_t read_uint32_le(uint8_t const* const stream, int64_t& pos) {
        pos += 4;
        return read_uint32_le(stream + pos - 4);
    }

    uint64_t read_uint64_le(uint8_t const* const stream, int64_t& pos) {
        pos += 8;
        return read_uint64_le(stream + pos - 8);
    }

    // Big endian read functions

    uint16_t read_uint16_be(uint8_t const* const stream) {
        return (static_cast<uint16_t>(stream[0]) << 8) | static_cast<uint16_t>(stream[1]);
    }

    uint32_t read_uint32_be(uint8_t const* const stream) {
        return (static_cast<uint32_t>(stream[0]) << 24) | (static_cast<uint32_t>(stream[1]) << 16) | (static_cast<uint32_t>(stream[2]) << 8) |
               static_cast<uint32_t>(stream[3]);
    }

    uint64_t read_uint64_be(uint8_t const* const stream) {
        return (static_cast<uint64_t>(stream[0]) << 56) | (static_cast<uint64_t>(stream[1]) << 48) | (static_cast<uint64_t>(stream[2]) << 40) |
               (static_cast<uint64_t>(stream[3]) << 32) | (static_cast<uint64_t>(stream[4]) << 24) | (static_cast<uint64_t>(stream[5]) << 16) |
               (static_cast<uint64_t>(stream[6]) << 8) | static_cast<uint64_t>(stream[7]);
    }

    uint16_t read_uint16_be(uint8_t const* const stream, int64_t& pos) {
        pos += 2;
        return read_uint16_be(stream + pos - 2);
    }

    uint32_t read_uint32_be(uint8_t const* const stream, int64_t& pos) {
        pos += 4;
        return read_uint32_be(stream + pos - 4);
    }

    uint64_t read_uint64_be(uint8_t const* const stream, int64_t& pos) {
        pos += 8;
        return read_uint64_be(stream + pos - 8);
    }
} // namespace anton_engine::importers
