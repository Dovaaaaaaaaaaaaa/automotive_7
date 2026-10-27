#include <iostream>
#include <string>
#include "sha256.hpp"
#include "util.hpp"

std::string sha256_hex(const std::string& text) {
    SHA256 h;
    h.update(reinterpret_cast<const std::uint8_t*>(text.data()), text.size());
    return to_hex(h.digest());
}

int main() {
    std::cout << "SHA-256(\"abc\")\n " << sha256_hex("abc") << "\n";
    std::cout << "SHA-256(\"\")\n " << sha256_hex("") << "\n";
    std::cout << "SHA-256(\"The quick brown fox jumps over the lazy dog\")\n "
              << sha256_hex("The quick brown fox jumps over the lazy dog") << "\n";
    
    // Сообщение длиной 56 байт (неудобный случай для padding)
    std::cout << "SHA-256(56 байт)\n "
              << sha256_hex("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq") << "\n";
              
    // Лавинный эффект
    std::cout << "\nЛавинный эффект:\n abc -> " << sha256_hex("abc") << "\n abd -> "
              << sha256_hex("abd") << "\n";
    return 0;
}
