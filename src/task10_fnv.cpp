#include <iostream>
#include "fnv.hpp"
#include "util.hpp"

int main() {
    std::cout << "FNV-1a 32(\"\") = " << hex_str(fnv1a_32("", 0), 8) << "\n";
    // Ожидается 0x811C9DC5
    
    std::cout << "FNV-1a 32(\"abc\") = " << hex_str(fnv1a_32("abc", 3), 8) << "\n";
    // Ожидается 0x1A47E90B
    
    std::cout << "FNV-1a 64(\"abc\") = " << hex_str(fnv1a_64("abc", 3), 16) << "\n";
    // Ожидается 0xE71FA2190541574B
    return 0;
}
