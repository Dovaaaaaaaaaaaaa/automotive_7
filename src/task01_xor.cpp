#include <iostream>
#include "xor_sum.hpp"

int main() {
    Bytes data = to_bytes("123456789");
    std::cout << "XOR(\"123456789\") = " << hex_str(xor_checksum(data), 2) << "\n";

    Bytes a = {0x01, 0x02};
    Bytes b = {0x02, 0x01};
    std::cout << "XOR([01 02]) = " << hex_str(xor_checksum(a), 2) << "\n";
    std::cout << "XOR([02 01]) = " << hex_str(xor_checksum(b), 2) << "\n";
    std::cout << "XOR не обнаруживает перестановку: "
              << (xor_checksum(a) == xor_checksum(b) ? "ДА" : "НЕТ") << "\n";

    Bytes c1 = {0x10, 0x20};
    Bytes c2 = {0x11, 0x21};
    std::cout << "XOR([10 20]) = " << hex_str(xor_checksum(c1), 2) << "\n";
    std::cout << "XOR([11 21]) = " << hex_str(xor_checksum(c2), 2) << "\n";
    std::cout << "XOR не обнаруживает две ошибки в одном бите: "
              << (xor_checksum(c1) == xor_checksum(c2) ? "ДА" : "НЕТ") << "\n";
    return 0;
}
