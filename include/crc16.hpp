#pragma once
#include <array>
#include "util.hpp"

// CRC-16/CCITT-FALSE: poly=0x1021, init=0xFFFF, без отражения, xorout=0
class Crc16CcittFalse {
public:
    Crc16CcittFalse() {
        for (std::uint32_t i = 0; i < 256; ++i) {
            std::uint16_t c = static_cast<std::uint16_t>(i << 8);
            for (int j = 0; j < 8; ++j) {
                c = (c & 0x8000) ? static_cast<std::uint16_t>((c << 1) ^ 0x1021)
                                 : static_cast<std::uint16_t>(c << 1);
            }
            table_[i] = c;
        }
    }
    
    std::uint16_t compute(const Bytes& data) const {
        std::uint16_t crc = 0xFFFF;
        for (auto b : data) {
            crc = static_cast<std::uint16_t>((crc << 8) ^ table_[((crc >> 8) ^ b) & 0xFF]);
        }
        return crc;
    }
private:
    std::array<std::uint16_t, 256> table_{};
};
