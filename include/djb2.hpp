#pragma once
#include <cstddef>
#include <cstdint>

inline std::uint32_t djb2(const void* data, std::size_t len) {
    std::uint32_t hash = 5381;
    const auto* p = static_cast<const std::uint8_t*>(data);
    for (std::size_t i = 0; i < len; ++i)
        hash = ((hash << 5) + hash) + p[i]; // hash * 33 + byte
    return hash;
}
