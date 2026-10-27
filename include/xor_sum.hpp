#pragma once
#include "util.hpp"

inline std::uint8_t xor_checksum(const Bytes& data) {
    std::uint8_t c = 0;
    for (auto b : data) c ^= b;
    return c;
}
