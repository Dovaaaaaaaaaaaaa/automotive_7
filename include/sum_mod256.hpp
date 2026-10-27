#pragma once
#include "util.hpp"

inline std::uint8_t sum_mod256(const Bytes& data) {
    std::uint8_t s = 0;
    for (auto b : data) s = static_cast<std::uint8_t>(s + b);
    return s;
}
