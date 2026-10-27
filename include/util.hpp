#pragma once

// Вспомогательные функции, общие для всех заданий.
#include <cstdint>
#include <string>
#include <vector>

using Bytes = std::vector<std::uint8_t>;

// Строка -> массив байт
inline Bytes to_bytes(const std::string& s) {
    return Bytes(s.begin(), s.end());
}

// Число -> строка вида 0x4B (digits = число шестнадцатеричных цифр)
inline std::string hex_str(std::uint64_t value, int digits) {
    const char* alphabet = "0123456789ABCDEF";
    std::string s(static_cast<std::size_t>(digits), '0');
    for (int i = digits - 1; i >= 0; --i) {
        s[static_cast<std::size_t>(i)] = alphabet[value & 0xF];
        value >>= 4;
    }
    return "0x" + s;
}

// Массив байт -> строка из шестнадцатеричных цифр без пробелов
template <typename Container>
std::string to_hex(const Container& d) {
    const char* alphabet = "0123456789abcdef";
    std::string s;
    for (auto b : d) {
        s += alphabet[(b >> 4) & 0xF];
        s += alphabet[b & 0xF];
    }
    return s;
}
