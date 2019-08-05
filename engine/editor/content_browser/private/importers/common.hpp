#ifndef EDITOR_ASSET_IMPORTER_IMPORTERS_COMMON_HPP_INCLUDE
#define EDITOR_ASSET_IMPORTER_IMPORTERS_COMMON_HPP_INCLUDE

#include <cstdint>

namespace importers {
    uint8_t read_uint8(uint8_t const* const stream, uint64_t& pos);

    // Little-endian read functions

    uint16_t read_uint16_le(uint8_t const* const stream);
    uint32_t read_uint32_le(uint8_t const* const stream);
    uint64_t read_uint64_le(uint8_t const* const stream);

    uint16_t read_uint16_le(uint8_t const* const stream, uint64_t& pos);
    uint32_t read_uint32_le(uint8_t const* const stream, uint64_t& pos);
    uint64_t read_uint64_le(uint8_t const* const stream, uint64_t& pos);

    // Big endian read functions

    uint16_t read_uint16_be(uint8_t const* const stream);
    uint32_t read_uint32_be(uint8_t const* const stream);
    uint64_t read_uint64_be(uint8_t const* const stream);

    uint16_t read_uint16_be(uint8_t const* const stream, uint64_t& pos);
    uint32_t read_uint32_be(uint8_t const* const stream, uint64_t& pos);
    uint64_t read_uint64_be(uint8_t const* const stream, uint64_t& pos);
} // namespace importers

#endif // !EDITOR_ASSET_IMPORTER_IMPORTERS_COMMON_HPP_INCLUDE
