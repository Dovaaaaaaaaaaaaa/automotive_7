#include <array>
#include <cstdint>
#include <iostream>
#include "e2e_crc.hpp"
#include "util.hpp"

struct CanFrame {
    std::uint32_t id = 0;          // CAN ID
    std::array<std::uint8_t, 8> data{}; // 8 байт данных; байт 7 хранит CRC
};

// CRC считается по байтам 0..6 и Data ID
std::uint8_t frame_crc(const CanFrame& f, std::uint16_t data_id) {
    return crc8_e2e_p01(f.data.data(), 7, data_id);
}

void print_frame(const CanFrame& f) {
    std::cout << "ID=" << hex_str(f.id, 3) << " data:";
    for (auto b : f.data) std::cout << " " << hex_str(b, 2).substr(2);
    std::cout << "\n";
}

int main() {
    const std::uint16_t data_id = 0x0010;
    CanFrame f;
    f.id = 0x100;
    f.data = {0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x00};
    f.data[7] = frame_crc(f, data_id); // отправитель вписывает CRC

    std::cout << "Отправлено:\n "; print_frame(f);

    // Приёмник проверяет кадр
    auto check = [&](const CanFrame& rx, std::uint16_t expected_id) {
        return frame_crc(rx, expected_id) == rx.data[7];
    };

    std::cout << "Исходный кадр принят приёмником: " << (check(f, data_id) ? "ДА" : "НЕТ") << "\n";

    // Помеха инвертировала один бит в байте 2
    CanFrame corrupted = f;
    corrupted.data[2] ^= 0x01;
    std::cout << "\nПосле искажения бита в байте 2:\n "; print_frame(corrupted);
    std::cout << "Искажение обнаружено: " << (!check(corrupted, data_id) ? "ДА" : "НЕТ") << "\n";

    // Тот же кадр пришёл от «чужого» отправителя (другой Data ID ожидается приёмником)
    std::cout << "\nПриёмник ожидает Data ID 0x0011, а кадр сформирован для 0x0010:\n";
    std::cout << "Несоответствие обнаружено: " << (!check(f, 0x0011) ? "ДА" : "НЕТ") << "\n";

    // Статистика: сколько из всех 56 однобитовых искажений данных (байты 0..6) обнаружено
    int detected = 0, total = 0;
    for (int byte = 0; byte < 7; ++byte) {
        for (int bit = 0; bit < 8; ++bit) {
            CanFrame c = f;
            c.data[static_cast<std::size_t>(byte)] ^= static_cast<std::uint8_t>(1u << bit);
            ++total;
            if (!check(c, data_id)) ++detected;
        }
    }

    std::cout << "\nОднобитовые искажения: обнаружено " << detected << " из " << total << "\n";
    return 0;
}
