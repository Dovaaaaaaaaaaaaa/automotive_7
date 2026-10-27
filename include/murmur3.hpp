#pragma once
#include <cstddef>
#include <cstdint>
#include <cstring>

inline std::uint32_t murmur3_32(const void* key, std::size_t len, std::uint32_t seed = 0) {
    const auto* data = static_cast<const std::uint8_t*>(key);
    const std::size_t nblocks = len / 4;
    std::uint32_t h1 = seed;
    const std::uint32_t c1 = 0xcc9e2d51u;
    const std::uint32_t c2 = 0x1b873593u;
    for (std::size_t i = 0; i < nblocks; ++i) {
        std::uint32_t k1;
        std::memcpy(&k1, data + i * 4, 4);
        k1 *= c1;
        k1 = (k1 << 15) | (k1 >> 17);
        k1 *= c2;
        h1 ^= k1;
        h1 = (h1 << 13) | (h1 >> 19);
        h1 = h1 * 5 + 0xe6546b64u;
    }
    const std::uint8_t* tail = data + nblocks * 4;
    std::uint32_t k1 = 0;
    switch (len & 3) {
        case 3: k1 ^= static_cast<std::uint32_t>(tail[2]) << 16; [[fallthrough]];
        case 2: k1 ^= static_cast<std::uint32_t>(tail[1]) << 8;  [[fallthrough]];
        case 1: k1 ^= tail[0];
                k1 *= c1;
                k1 = (k1 << 15) | (k1 >> 17);
                k1 *= c2;
                h1 ^= k1;
    }
    h1 ^= static_cast<std::uint32_t>(len);
    h1 ^= h1 >> 16;
    h1 *= 0x85ebca6bu;
    h1 ^= h1 >> 13;
    h1 *= 0xc2b2ae35u;
    h1 ^= h1 >> 16;
    return h1;
}
