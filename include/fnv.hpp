#pragma once
#include <cstddef>
#include <cstdint>

inline std::uint32_t fnv1a_32(const void* data, std::size_t len) {
    const std::uint32_t OFFSET = 0x811C9DC5u;
    const std::uint32_t PRIME = 0x01000193u;
    std::uint32_t hash = OFFSET;
    const auto* p = static_cast<const std::uint8_t*>(data);
    for (std::size_t i = 0; i < len; ++i) {
        hash ^= p[i];
        hash *= PRIME;
    }
    return hash;
}

inline std::uint64_t fnv1a_64(const void* data, std::size_t len) {
    const std::uint64_t OFFSET = 0xCBF29CE484222325ULL;
    const std::uint64_t PRIME = 0x100000001B3ULL;
    std::uint64_t hash = OFFSET;
    const auto* p = static_cast<const std::uint8_t*>(data);
    for (std::size_t i = 0; i < len; ++i) {
        hash ^= p[i];
        hash *= PRIME;
    }
    return hash;
}
