#include <iostream>
#include "fletcher16.hpp"

int main() {
    Bytes data = to_bytes("123456789");
    std::cout << "Fletcher-16(\"123456789\") = " << hex_str(fletcher16(data), 4) << "\n";

    Bytes a = {0x01, 0x02};
    Bytes b = {0x02, 0x01};
    std::cout << "Fletcher([01 02]) = " << hex_str(fletcher16(a), 4) << "\n";
    std::cout << "Fletcher([02 01]) = " << hex_str(fletcher16(b), 4) << "\n";
    std::cout << "Перестановка обнаружена: "
              << (fletcher16(a) != fletcher16(b) ? "ДА" : "НЕТ") << "\n";

    Bytes zeros = {0x00, 0x00, 0x00, 0x00};
    Bytes ones  = {0xFF, 0xFF, 0xFF, 0xFF};
    std::cout << "Fletcher([00 00 00 00]) = " << hex_str(fletcher16(zeros), 4) << "\n";
    std::cout << "Fletcher([FF FF FF FF]) = " << hex_str(fletcher16(ones), 4) << "\n";
    std::cout << "Блоки из нулей и из 0xFF не различаются: "
              << (fletcher16(zeros) == fletcher16(ones) ? "ДА" : "НЕТ") << "\n";
    return 0;
}
