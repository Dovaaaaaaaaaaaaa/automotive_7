#pragma once
#include <cstddef>
#include <cstdint>

// Упрощённая модель CRC для AUTOSAR E2E Profile 1:
// CRC-8 с полиномом 0x1D, init = 0x00, без xorout;
// сначала идут байты данных, затем два байта Data ID (старший, младший).
inline std::uint8_t crc8_e2e_p01(const std::uint8_t* data, std::size_t len,
                                 std::uint16_t data_id) {
    std::uint8_t crc = 0x00;
    auto feed = [&crc](std::uint8_t b) {
        crc ^= b;
        for (int i = 0; i < 8; ++i)
            crc = (crc & 0x80) ? static_cast<std::uint8_t>((crc << 1) ^ 0x1D)
                               : static_cast<std::uint8_t>(crc << 1);
    };
    for (std::size_t i = 0; i < len; ++i) feed(data[i]);
    feed(static_cast<std::uint8_t>(data_id >> 8));
    feed(static_cast<std::uint8_t>(data_id & 0xFF));
    return crc;
}
