#include <importers/common.hpp>

namespace importers {
    uint8_t read_uint8(uint8_t const* const stream, uint64_t& pos) {
        pos += 1;
        return *(stream + pos - 1);
    }

    // Little-endian read functions

    uint16_t read_uint16_le(uint8_t const* const stream) {
        return static_cast<uint16_t>(*stream) | (static_cast<uint16_t>(*(stream + 1)) << 8);
    }

    uint32_t read_uint32_le(uint8_t const* const stream) {
        return static_cast<uint32_t>(read_uint16_le(stream)) | (static_cast<uint32_t>(read_uint16_le(stream + 2)) << 16);
    }

    uint64_t read_uint64_le(uint8_t const* const stream) {
        return static_cast<uint64_t>(read_uint32_le(stream)) | (static_cast<uint64_t>(read_uint32_le(stream + 4)) << 32);
    }

    uint16_t read_uint16_le(uint8_t const* const stream, uint64_t& pos) {
        return static_cast<uint16_t>(read_uint8(stream, pos)) | (static_cast<uint16_t>(read_uint8(stream, pos)) << 8);
    }

    uint32_t read_uint32_le(uint8_t const* const stream, uint64_t& pos) {
        return static_cast<uint32_t>(read_uint16_le(stream, pos)) | (static_cast<uint32_t>(read_uint16_le(stream, pos)) << 16);
    }

    uint64_t read_uint64_le(uint8_t const* const stream, uint64_t& pos) {
        return static_cast<uint64_t>(read_uint32_le(stream, pos)) | (static_cast<uint64_t>(read_uint32_le(stream, pos)) << 32);
    }

    // Big endian read functions

    uint16_t read_uint16_be(uint8_t const* const stream) {
        return (static_cast<uint16_t>(*stream) << 8) | static_cast<uint16_t>(*(stream + 1));
    }

    uint32_t read_uint32_be(uint8_t const* const stream) {
        return (static_cast<uint32_t>(read_uint16_be(stream)) << 16) | static_cast<uint32_t>(read_uint16_be(stream + 2));
    }

    uint64_t read_uint64_be(uint8_t const* const stream) {
        return (static_cast<uint64_t>(read_uint32_be(stream)) << 32) | static_cast<uint64_t>(read_uint32_be(stream + 4));
    }

    uint16_t read_uint16_be(uint8_t const* const stream, uint64_t& pos) {
        return (static_cast<uint16_t>(read_uint8(stream, pos)) << 8) | static_cast<uint16_t>(read_uint8(stream, pos));
    }

    uint32_t read_uint32_be(uint8_t const* const stream, uint64_t& pos) {
        return (static_cast<uint32_t>(read_uint16_be(stream, pos)) << 16) | static_cast<uint32_t>(read_uint16_be(stream, pos));
    }

    uint64_t read_uint64_be(uint8_t const* const stream, uint64_t& pos) {
        return (static_cast<uint64_t>(read_uint32_be(stream, pos)) << 32) | static_cast<uint64_t>(read_uint32_be(stream, pos));
    }
} // namespace importers