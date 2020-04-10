#ifndef CORE_HASHING_MURMURHASH2_HPP_INCLUDE
#define CORE_HASHING_MURMURHASH2_HPP_INCLUDE

#include <core/types.hpp>

namespace anton_engine {
    //-----------------------------------------------------------------------------
    // MurmurHash2 was written by Austin Appleby, and is placed in the public
    // domain. The author hereby disclaims copyright to this source code.
    //
    // Note - This code makes a few assumptions about how your machine behaves -
    //
    // 1. We can read a 4-byte value from any address without crashing
    // 2. sizeof(int) == 4
    //
    // And it has a few limitations -
    //
    // 1. It will not work incrementally.
    // 2. It will not produce the same results on little-endian and big-endian
    //    machines.
    // 
    // len - size of the data pointed to by key in bytes.
    //
    constexpr u32 murmurhash2_32(void const* key, i32 len, u32 seed) {
        #define mmix(h,k) { k *= m; k ^= k >> r; k *= m; h *= m; h ^= k; }

        // 'm' and 'r' are mixing constants generated offline.
        // They're not really 'magic', they just happen to work well.
        u32 const m = 0x5bd1e995;
        i32 const r = 24;
        u32 l = len;

        // Mix 4 bytes at a time into the hash

        unsigned char const* data = (unsigned char const*)key;

        u32 h = seed;

        while(len >= 4) {
            u32 k = *(u32*)data;

            mmix(h,k);

            data += 4;
            len -= 4;
        }

        u32 t = 0;

        // Handle the last few bytes of the input array

        switch(len) {
            case 3: 
                t ^= data[2] << 16;
            case 2: 
                t ^= data[1] << 8;
            case 1: 
                t ^= data[0];
        }

        mmix(h, t);
        mmix(h, l);

        // Do a few final mixes of the hash to ensure the last few
        // bytes are well-incorporated.

        h ^= h >> 13;
        h *= m;
        h ^= h >> 15;

        #undef mmix

        return h;
    }

    // MurmurHash2, 64-bit hash, by Austin Appleby
    //
    // The same caveats as 32-bit MurmurHash2 apply here - beware of alignment
    // and endian-ness issues if used across multiple platforms.
    //
    // len - size of the data pointed to by key in bytes.
    //
    constexpr u64 murmurhash2_64(void const* key, i64 len, u64 seed) {
        u64 const m = 0xc6a4a7935bd1e995;
        i64 const r = 47;

        u64 h = seed ^ (len * m);

        u64 const* data = (u64 const*)key;
        u64 const* end = data + (len / 8);

        while (data != end) {
            u64 k = *data++;

            k *= m;
            k ^= k >> r;
            k *= m;

            h ^= k;
            h *= m;
        }

        unsigned char const* data2 = (unsigned char const*)data;

        switch (len & 7) {
        case 7:
            h ^= u64(data2[6]) << 48;
        case 6:
            h ^= u64(data2[5]) << 40;
        case 5:
            h ^= u64(data2[4]) << 32;
        case 4:
            h ^= u64(data2[3]) << 24;
        case 3:
            h ^= u64(data2[2]) << 16;
        case 2:
            h ^= u64(data2[1]) << 8;
        case 1:
            h ^= u64(data2[0]);
            h *= m;
        };

        h ^= h >> r;
        h *= m;
        h ^= h >> r;

        return h;
    }
} // namespace anton_engine

#endif // !CORE_HASHING_MURMURHASH2_HPP_INCLUDE
