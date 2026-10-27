#include <iostream>
#include "crc16.hpp"
#include "crc32.hpp"

int main() {
    Bytes data = to_bytes("123456789");
    Crc16CcittFalse crc16;
    Crc32IsoHdlc crc32;

    std::cout << "CRC-16/CCITT-FALSE = " << hex_str(crc16.compute(data), 4) << "\n";
    std::cout << "CRC-32/ISO-HDLC = " << hex_str(crc32.compute(data), 8) << "\n";
    return 0;
}
