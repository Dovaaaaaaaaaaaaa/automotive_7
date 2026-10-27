#include <iostream>
#include <string>
#include "sha256.hpp"
#include "util.hpp"

Bytes sha256(const Bytes& msg) {
    SHA256 s;
    s.update(msg.data(), msg.size());
    auto d = s.digest();
    return Bytes(d.begin(), d.end());
}

Bytes hmac_sha256(const Bytes& key, const Bytes& msg) {
    constexpr std::size_t B = 64; // размер блока SHA-256 в байтах
    Bytes k = key;
    if (k.size() > B) k = sha256(k); // длинный ключ сначала хешируется
    k.resize(B, 0);                  // и дополняется нулями до размера блока

    Bytes ipad(B), opad(B);
    for (std::size_t i = 0; i < B; ++i) {
        ipad[i] = static_cast<std::uint8_t>(k[i] ^ 0x36);
        opad[i] = static_cast<std::uint8_t>(k[i] ^ 0x5C);
    }

    Bytes inner_input = ipad; // (K xor ipad) || msg
    inner_input.insert(inner_input.end(), msg.begin(), msg.end());
    Bytes inner_hash = sha256(inner_input);

    Bytes outer_input = opad; // (K xor opad) || H(...)
    outer_input.insert(outer_input.end(), inner_hash.begin(), inner_hash.end());
    return sha256(outer_input);
}

int main() {
    // Проверка по тестовому вектору RFC 4231 (случай 1)
    Bytes rfc_key(20, 0x0b);
    Bytes rfc_msg = to_bytes("Hi There");
    std::cout << "HMAC-SHA256 (RFC 4231, тест 1):\n "
              << to_hex(hmac_sha256(rfc_key, rfc_msg)) << "\n\n";

    Bytes key      = to_bytes("secret_key_1234");
    Bytes msg_orig = to_bytes("ECU_calib_v1");
    Bytes msg_fake = to_bytes("ECU_calib_v2");

    // --- Сценарий 1: защита БЕЗ ключа (простой хеш) ---
    // Отправитель шлёт пару (сообщение, SHA-256(сообщение)).
    // Атакующий подменяет сообщение и просто пересчитывает хеш.
    Bytes tag_orig_plain = sha256(msg_orig);
    Bytes tag_fake_plain = sha256(msg_fake); // атакующий вычислил сам

    std::cout << "Простой хеш: приёмник проверяет (msg_fake, hash_fake)\n";
    std::cout << " хеш подделки = " << to_hex(tag_fake_plain).substr(0, 32) << "...\n";
    std::cout << " Приёмник принимает подделку: "
              << (sha256(msg_fake) == tag_fake_plain ? "ДА (защиты нет)" : "НЕТ") << "\n\n";

    // --- Сценарий 2: HMAC с секретным ключом ---
    Bytes mac_orig = hmac_sha256(key, msg_orig);
    Bytes mac_fake = hmac_sha256(key, msg_fake);

    std::cout << "HMAC(orig) = " << to_hex(mac_orig).substr(0, 32) << "...\n";
    std::cout << "HMAC(fake) = " << to_hex(mac_fake).substr(0, 32) << "...\n";
    std::cout << "MAC совпадают: " << (mac_orig == mac_fake ? "ДА (плохо)" : "НЕТ (хорошо)") << "\n\n";

    // Атакующий не знает ключ и пробует свой
    Bytes wrong_key = to_bytes("wrong_key");
    Bytes mac_forged = hmac_sha256(wrong_key, msg_fake);

    std::cout << "Атакующий присылает (msg_fake, MAC на неверном ключе)\n";
    std::cout << " Приёмник считает HMAC(key, msg_fake) = " << to_hex(mac_fake).substr(0, 32) << "...\n";
    std::cout << " Приёмник принимает подделку: "
              << (mac_forged == mac_fake ? "ДА (плохо)" : "НЕТ (хорошо)") << "\n";
    return 0;
}
