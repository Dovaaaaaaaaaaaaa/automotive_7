#pragma once
#include <array>
#include "util.hpp"

// Параметры CRC по каталогу Greg Cook (reveng)
struct CrcParams {
    const char* name;
    int width;              // 8, 16 или 32
    std::uint32_t poly;     // полином без старшего бита
    std::uint32_t init;
    bool refin;             // отражать входные байты
    bool refout;            // отражать результат
    std::uint32_t xorout;
};

// Универсальный табличный CRC для width <= 32
class Crc {
public:
    explicit Crc(const CrcParams& p) : p_(p) {
        mask_ = (p.width == 32) ? 0xFFFFFFFFu : ((1u << p.width) - 1u);
        top_ = 1u << (p.width - 1);
        for (std::uint32_t i = 0; i < 256; ++i) {
            // работаем в «прямом» (нереверсированном) виде; refin учитывается при обработке байта
            std::uint32_t c = i << (p.width - 8);
            for (int j = 0; j < 8; ++j)
                c = (c & top_) ? ((c << 1) ^ p.poly) : (c << 1);
            table_[i] = c & mask_;
        }
    }

    std::uint32_t compute(const Bytes& data) const {
        std::uint32_t crc = p_.init & mask_;
        for (auto byte : data) {
            std::uint8_t b = p_.refin ? reflect(byte, 8) : byte;
            std::uint32_t idx = ((crc >> (p_.width - 8)) ^ b) & 0xFF;
            crc = ((crc << 8) ^ table_[idx]) & mask_;
        }
        if (p_.refout) crc = reflect(crc, p_.width);
        return (crc ^ p_.xorout) & mask_;
    }
    const CrcParams& params() const { return p_; }

private:
    // Отразить младшие nbits бит числа: 0b0001 -> 0b1000
    static std::uint32_t reflect(std::uint32_t v, int nbits) {
        std::uint32_t r = 0;
        for (int i = 0; i < nbits; ++i) {
            r = (r << 1) | (v & 1u);
            v >>= 1;
        }
        return r;
    }
    CrcParams p_;
    std::uint32_t mask_ = 0, top_ = 0;
    std::array<std::uint32_t, 256> table_{};
};

// Стандартные профили
namespace profiles {
const CrcParams CRC8_SAE_J1850 {"CRC-8/SAE-J1850", 8, 0x1D, 0xFF, false, false, 0xFF};
const CrcParams CRC8_AUTOSAR {"CRC-8/AUTOSAR", 8, 0x2F, 0xFF, false, false, 0xFF};
const CrcParams CRC16_CCITT_FALSE {"CRC-16/CCITT-FALSE", 16, 0x1021, 0xFFFF, false, false, 0x0000};
const CrcParams CRC16_ARC {"CRC-16/ARC", 16, 0x8005, 0x0000, true, true, 0x0000};
const CrcParams CRC32_ISO_HDLC {"CRC-32/ISO-HDLC", 32, 0x04C11DB7, 0xFFFFFFFF, true, true, 0xFFFFFFFF};
const CrcParams CRC32C {"CRC-32C", 32, 0x1EDC6F41, 0xFFFFFFFF, true, true, 0xFFFFFFFF};
// Профиль, который просят добавить в задании:
const CrcParams CRC16_KERMIT {"CRC-16/KERMIT", 16, 0x1021, 0x0000, true, true, 0x0000};
} // namespace profiles
