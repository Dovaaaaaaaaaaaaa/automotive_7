#pragma once
#include "util.hpp"

inline std::uint32_t adler32(const Bytes& data) {
    constexpr std::uint32_t MOD = 65521;
    std::uint32_t a = 1, b = 0;
    for (auto byte : data) {
        a = (a + byte) % MOD;
        b = (b + a) % MOD;
    }
    return (b << 16) | a;
}
