#pragma once
#include "util.hpp"

// CRC-8/SAE-J1850, побитовая реализация: poly=0x1D, init=0xFF, xorout=0xFF
inline std::uint8_t crc8_sae_j1850(const Bytes& data) {
    const std::uint8_t POLY = 0x1D;
    const std::uint8_t INIT = 0xFF;
    const std::uint8_t XOROUT = 0xFF;
    std::uint8_t crc = INIT;
    for (auto b : data) {
        crc ^= b;
        for (int i = 0; i < 8; ++i) {
            if (crc & 0x80) {
                crc = static_cast<std::uint8_t>((crc << 1) ^ POLY);
            } else {
                crc = static_cast<std::uint8_t>(crc << 1);
            }
        }
    }
    return static_cast<std::uint8_t>(crc ^ XOROUT);
}
