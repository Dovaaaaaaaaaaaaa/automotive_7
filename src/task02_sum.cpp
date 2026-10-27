#include <iostream>
#include "sum_mod256.hpp"

int main() {
    Bytes data = to_bytes("123456789");
    std::cout << "SUM mod 256(\"123456789\") = " << hex_str(sum_mod256(data), 2) << "\n";

    Bytes a = {0x10, 0x20, 0x30};
    Bytes b = {0x11, 0x20, 0x2F};
    std::cout << "SUM([10 20 30]) = " << hex_str(sum_mod256(a), 2) << "\n";
    std::cout << "SUM([11 20 2F]) = " << hex_str(sum_mod256(b), 2) << "\n";
    std::cout << "Компенсирующие ошибки не обнаружены: "
              << (sum_mod256(a) == sum_mod256(b) ? "ДА" : "НЕТ") << "\n";

    Bytes c = {0x01, 0x02};
    Bytes d = {0x02, 0x01};
    std::cout << "Перестановка не обнаружена: "
              << (sum_mod256(c) == sum_mod256(d) ? "ДА" : "НЕТ") << "\n";
    return 0;
}
