#include <array>
#include <iostream>
#include <random>
#include "crc32.hpp"
#include "sha256.hpp"
#include "util.hpp"

using Sha = std::array<std::uint8_t, 32>;

Sha sha256_of(const Bytes& d) {
    SHA256 s;
    s.update(d.data(), d.size());
    return s.digest();
}

// ---------- Подгонка CRC-32 (то, что умеет делать атакующий) ----------
std::array<std::uint32_t, 256> make_table() {
    std::array<std::uint32_t, 256> t{};
    for (std::uint32_t i = 0; i < 256; ++i) {
        std::uint32_t c = i;
        for (int j = 0; j < 8; ++j) c = (c & 1) ? (c >> 1) ^ 0xEDB88320u : c >> 1;
        t[i] = c;
    }
    return t;
}

std::array<std::uint8_t, 4> forge_crc32(const Bytes& prefix, std::uint32_t target) {
    static const auto T = make_table();
    std::uint32_t state = 0xFFFFFFFFu;
    for (auto b : prefix) state = T[(state ^ b) & 0xFF] ^ (state >> 8);
    std::uint32_t want = target ^ 0xFFFFFFFFu;
    
    std::uint8_t idx[4];
    std::uint32_t c = want;
    for (int k = 3; k >= 0; --k) {
        std::uint8_t i = 0;
        for (int cand = 0; cand < 256; ++cand)
            if ((T[static_cast<std::size_t>(cand)] >> 24) == (c >> 24)) i = static_cast<std::uint8_t>(cand);
        idx[k] = i;
        c = (c ^ T[i]) << 8;
    }
    
    std::array<std::uint8_t, 4> out{};
    for (std::size_t k = 0; k < 4; ++k) {
        out[k] = static_cast<std::uint8_t>(idx[k] ^ (state & 0xFF));
        state = T[idx[k]] ^ (state >> 8);
    }
    return out;
}

int main() {
    // «Прошивка» 64 КБ
    Bytes firmware(64 * 1024);
    std::mt19937 rng(42);
    for (auto& b : firmware) b = static_cast<std::uint8_t>(rng() & 0xFF);

    Crc32IsoHdlc crc32;
    std::uint32_t crc_expected = crc32.compute(firmware);
    Sha sha_expected = sha256_of(firmware);
    std::cout << "Эталонный CRC-32 = " << hex_str(crc_expected, 8) << "\n";
    std::cout << "Эталонный SHA-256 = " << to_hex(sha_expected) << "\n";

    // --- Сценарий A: случайное повреждение одного бита ---
    Bytes fwA = firmware;
    fwA[12345] ^= 0x01;
    std::cout << "\nСценарий A: случайное повреждение\n";
    std::cout << " CRC-32 совпадает с эталоном: " << (crc32.compute(fwA) == crc_expected ? "ДА" : "НЕТ") << "\n";
    std::cout << " SHA-256 совпадает с эталоном: " << (sha256_of(fwA) == sha_expected ? "ДА" : "НЕТ") << "\n";

    // --- Сценарий B: целенаправленная подмена, CRC пересчитан ---
    Bytes fwB = firmware;
    fwB[5000] = 0x90; fwB[5001] = 0x90; fwB[5002] = 0x90; // «вредоносный» код
    std::cout << "\nСценарий B: подмена кода и пересчёт CRC\n";
    std::cout << " Новый CRC-32 = " << hex_str(crc32.compute(fwB), 8)
              << " (атакующий просто записывает его вместо старого)\n";

    // --- Сценарий C: подмена с сохранением прежнего CRC-32 ---
    Bytes fwC = firmware;
    fwC[5000] = 0x90; fwC[5001] = 0x90; fwC[5002] = 0x90;
    
    Bytes prefix(fwC.begin(), fwC.end() - 4);
    auto patch = forge_crc32(prefix, crc_expected);
    for (std::size_t i = 0; i < 4; ++i) fwC[fwC.size() - 4 + i] = patch[i];
    
    std::cout << "\nСценарий C: подмена с подгонкой CRC\n";
    std::cout << " Образ изменён: " << (fwC != firmware ? "ДА" : "НЕТ") << "\n";
    std::cout << " CRC-32 образа = " << hex_str(crc32.compute(fwC), 8) << "\n";
    std::cout << " CRC-32 совпадает с эталоном: "
              << (crc32.compute(fwC) == crc_expected ? "ДА (проверка пройдена!)" : "НЕТ") << "\n";
    std::cout << " SHA-256 совпадает с эталоном: "
              << (sha256_of(fwC) == sha_expected ? "ДА" : "НЕТ (подмена обнаружена)") << "\n";
    return 0;
}
