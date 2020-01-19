#ifndef CORE_HASHINH_MURMURHASH2_HPP_INCLUDE
#define CORE_HASHINH_MURMURHASH2_HPP_INCLUDE

#include <anton_int.hpp>

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
    inline u32 murmurhash2_32(const void* key, int len, u32 seed) {
        // 'm' and 'r' are mixing constants generated offline.
        // They're not really 'magic', they just happen to work well.

        const u32 m = 0x5bd1e995;
        const int r = 24;

        // Initialize the hash to a 'random' value

        u32 h = seed ^ len;

        // Mix 4 bytes at a time into the hash

        const unsigned char* data = (const unsigned char*)key;

        while (len >= 4) {
            u32 k = *(u32*)data;

            k *= m;
            k ^= k >> r;
            k *= m;

            h *= m;
            h ^= k;

            data += 4;
            len -= 4;
        }

        // Handle the last few bytes of the input array

        switch (len) {
        case 3:
            h ^= data[2] << 16;
        case 2:
            h ^= data[1] << 8;
        case 1:
            h ^= data[0];
            h *= m;
        };

        // Do a few final mixes of the hash to ensure the last few
        // bytes are well-incorporated.

        h ^= h >> 13;
        h *= m;
        h ^= h >> 15;

        return h;
    }

    // MurmurHash2, 64-bit hash, by Austin Appleby
    //
    // The same caveats as 32-bit MurmurHash2 apply here - beware of alignment
    // and endian-ness issues if used across multiple platforms.
    //
    inline u64 murmurhash2_64(const void* key, int len, unsigned int seed) {
        const u64 m = 0xc6a4a7935bd1e995;
        const int r = 47;

        u64 h = seed ^ (len * m);

        const u64* data = (const u64*)key;
        const u64* end = data + (len / 8);

        while (data != end) {
            u64 k = *data++;

            k *= m;
            k ^= k >> r;
            k *= m;

            h ^= k;
            h *= m;
        }

        const unsigned char* data2 = (const unsigned char*)data;

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

#endif // !CORE_HASHINH_MURMURHASH2_HPP_INCLUDE
