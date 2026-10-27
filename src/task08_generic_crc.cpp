#include <iostream>
#include "crc16.hpp"
#include "crc32.hpp"
#include "crc8_table.hpp"
#include "crc_generic.hpp"

struct TestVector {
    const CrcParams* params;
    std::uint32_t expected;
};

int main() {
    Bytes data = to_bytes("123456789");
    // Массив тестов (добавлен KERMIT)
    const TestVector tests[] = {
        {&profiles::CRC8_SAE_J1850, 0x4B},
        {&profiles::CRC8_AUTOSAR, 0xDF},
        {&profiles::CRC16_CCITT_FALSE, 0x29B1},
        {&profiles::CRC16_ARC, 0xBB3D},
        {&profiles::CRC32_ISO_HDLC, 0xCBF43926},
        {&profiles::CRC32C, 0xE3069283},
        {&profiles::CRC16_KERMIT, 0x2189},
    };

    int passed = 0, failed = 0;
    for (const auto& t : tests) {
        Crc crc(*t.params);
        std::uint32_t actual = crc.compute(data);
        bool ok = (actual == t.expected);
        std::cout << (ok ? "[OK] " : "[FAIL] ") << t.params->name
                  << " ожидалось " << hex_str(t.expected, t.params->width / 4)
                  << " получено " << hex_str(actual, t.params->width / 4) << "\n";
        (ok ? passed : failed)++;
    }
    std::cout << "\nПройдено: " << passed << ", не пройдено: " << failed << "\n";

    // Сверка универсального класса с «ручными» реализациями на случайных данных
    Bytes blob(1000);
    for (std::size_t i = 0; i < blob.size(); ++i)
        blob[i] = static_cast<std::uint8_t>((i * 37 + 11) & 0xFF);

    bool same =
        Crc(profiles::CRC8_SAE_J1850).compute(blob) == Crc8SaeJ1850{}.compute(blob) &&
        Crc(profiles::CRC16_CCITT_FALSE).compute(blob) == Crc16CcittFalse{}.compute(blob) &&
        Crc(profiles::CRC32_ISO_HDLC).compute(blob) == Crc32IsoHdlc{}.compute(blob);

    std::cout << "Универсальный класс совпал с ручными реализациями: "
              << (same ? "ДА" : "НЕТ") << "\n";
    return failed == 0 ? 0 : 1;
}
