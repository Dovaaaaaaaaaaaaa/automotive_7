#include <iostream>
#include "crc8_bitwise.hpp"

int main() {
    Bytes data = to_bytes("123456789");
    std::cout << "CRC-8/SAE-J1850(\"123456789\") = "
              << hex_str(crc8_sae_j1850(data), 2) << "\n";
    return 0;
}
