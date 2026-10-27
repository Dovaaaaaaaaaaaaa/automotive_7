#include <iostream>
#include "adler32.hpp"

int main() {
    Bytes data = to_bytes("123456789");
    std::cout << "Adler-32(\"123456789\") = " << hex_str(adler32(data), 8) << "\n";

    Bytes one = {'A'};
    std::cout << "Adler-32(\"A\") = " << hex_str(adler32(one), 8) << "\n";
    return 0;
}
