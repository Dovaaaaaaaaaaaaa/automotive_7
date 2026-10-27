#include <cstring>
#include <iostream>
#include "djb2.hpp"
#include "murmur3.hpp"
#include "util.hpp"

int main() {
    const char* s = "The quick brown fox jumps over the lazy dog";
    std::size_t len = std::strlen(s);
    
    std::cout << "DJB2 = " << hex_str(djb2(s, len), 8) << "\n";
    std::cout << "MurmurHash3 = " << hex_str(murmur3_32(s, len), 8) << "\n";
    
    // Лавинный эффект: меняем один символ
    const char* s2 = "The quick brown fox jumps over the lazy cog";
    std::cout << "\nОдна буква изменена (dog -> cog):\n";
    std::cout << "DJB2 = " << hex_str(djb2(s2, len), 8) << "\n";
    std::cout << "MurmurHash3 = " << hex_str(murmur3_32(s2, len), 8) << "\n";
    return 0;
}
