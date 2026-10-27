#pragma once
#include <array>
#include "util.hpp"

// CRC-8/SAE-J1850, табличная реализация
class Crc8SaeJ1850 {
public:
    Crc8SaeJ1850() {
        for (int i = 0; i < 256; ++i) {
            std::uint8_t c = static_cast<std::uint8_t>(i);
            for (int j = 0; j < 8; ++j) {
                c = (c & 0x80) ? static_cast<std::uint8_t>((c << 1) ^ 0x1D)
                               : static_cast<std::uint8_t>(c << 1);
            }
            table_[static_cast<std::size_t>(i)] = c;
        }
    }
    
    std::uint8_t compute(const Bytes& data) const {
        std::uint8_t crc = 0xFF;
        for (auto b : data) {
            crc = table_[static_cast<std::size_t>(crc ^ b)];
        }
        return static_cast<std::uint8_t>(crc ^ 0xFF);
    }
private:
    std::array<std::uint8_t, 256> table_{};
};
