#pragma once
#include <array>
#include "util.hpp"

// CRC-32/ISO-HDLC (тот самый CRC-32 из zip, Ethernet):
class Crc32IsoHdlc {
public:
    Crc32IsoHdlc() {
        for (std::uint32_t i = 0; i < 256; ++i) {
            std::uint32_t c = i;
            for (int j = 0; j < 8; ++j) {
                c = (c & 1) ? (c >> 1) ^ 0xEDB88320u : c >> 1;
            }
            table_[i] = c;
        }
    }
    
    std::uint32_t compute(const Bytes& data) const {
        std::uint32_t crc = 0xFFFFFFFFu;
        for (auto b : data) {
            crc = table_[(crc ^ b) & 0xFF] ^ (crc >> 8);
        }
        return crc ^ 0xFFFFFFFFu;
    }
private:
    std::array<std::uint32_t, 256> table_{};
};
