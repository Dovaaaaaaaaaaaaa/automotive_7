#pragma once
#include "util.hpp"

inline std::uint16_t fletcher16(const Bytes& data) {
    std::uint32_t sum1 = 0, sum2 = 0;
    for (auto b : data) {
        sum1 = (sum1 + b) % 255;
        sum2 = (sum2 + sum1) % 255;
    }
    return static_cast<std::uint16_t>((sum2 << 8) | sum1);
}
