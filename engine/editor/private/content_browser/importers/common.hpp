#pragma once

#include <core/types.hpp>

namespace anton_engine::importers {
  u8 read_uint8(u8 const* const stream, i64& pos);

  // Little-endian read functions

  u16 read_uint16_le(u8 const* const stream);
  u32 read_uint32_le(u8 const* const stream);
  u64 read_uint64_le(u8 const* const stream);

  u16 read_uint16_le(u8 const* const stream, i64& pos);
  u32 read_uint32_le(u8 const* const stream, i64& pos);
  u64 read_uint64_le(u8 const* const stream, i64& pos);

  // Big endian read functions

  u16 read_uint16_be(u8 const* const stream);
  u32 read_uint32_be(u8 const* const stream);
  u64 read_uint64_be(u8 const* const stream);

  u16 read_uint16_be(u8 const* const stream, i64& pos);
  u32 read_uint32_be(u8 const* const stream, i64& pos);
  u64 read_uint64_be(u8 const* const stream, i64& pos);
} // namespace anton_engine::importers
