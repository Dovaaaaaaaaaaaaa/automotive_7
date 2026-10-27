#include <iostream>
#include "e2e_crc.hpp"
#include "util.hpp"

int main() {
    Bytes payload = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06};
    std::uint8_t crc_id_10 = crc8_e2e_p01(payload.data(), payload.size(), 0x0010);
    std::uint8_t crc_id_11 = crc8_e2e_p01(payload.data(), payload.size(), 0x0011);

    std::cout << "Payload: 01 02 03 04 05 06\n";
    std::cout << "CRC (Data ID = 0x0010): " << hex_str(crc_id_10, 2) << "\n";
    std::cout << "CRC (Data ID = 0x0011): " << hex_str(crc_id_11, 2) << "\n";
    std::cout << "CRC зависит от Data ID: " << (crc_id_10 != crc_id_11 ? "ДА" : "НЕТ") << "\n";

    // Приёмник, ожидающий сообщение с Data ID 0x0010, получил сообщение
    // от другого отправителя (Data ID 0x0011) с тем же payload. Проверка CRC его отвергнет.
    bool accepted = (crc8_e2e_p01(payload.data(), payload.size(), 0x0010) == crc_id_11);
    std::cout << "Приёмник (ждёт Data ID 0x0010) принял сообщение с Data ID 0x0011: "
              << (accepted ? "ДА (плохо)" : "НЕТ (хорошо)") << "\n";
    return 0;
}
