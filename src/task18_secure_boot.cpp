#include <array>
#include <iostream>
#include <string>
#include "sha256.hpp"
#include "util.hpp"

using Digest = std::array<std::uint8_t, 32>;

Digest sha256_of(const Bytes& d) {
    SHA256 s;
    s.update(d.data(), d.size());
    return s.digest();
}

struct SecureImage {
    std::string name;
    Bytes data;
    Digest expected_hash; // «эталон»: в реальном ECU хранится в защищённой памяти
};

bool verify(const SecureImage& img) {
    Digest actual = sha256_of(img.data);
    if (actual == img.expected_hash) {
        std::cout << " [" << img.name << "] Secure Boot OK\n";
        return true;
    }
    std::cout << " [" << img.name << "] Secure Boot FAILED: загрузка остановлена\n";
    return false;
}

int main() {
    Bytes bootloader(1024, 0xAA);
    Bytes kernel(2048, 0xBB);
    Bytes app(4096, 0xCC);
    
    SecureImage bl{"bootloader", bootloader, sha256_of(bootloader)};
    SecureImage kn{"kernel", kernel, sha256_of(kernel)};
    SecureImage ap{"app", app, sha256_of(app)};

    std::cout << "Проверка цепочки доверия:\n";
    if (!verify(bl)) return 1;
    if (!verify(kn)) return 1;
    if (!verify(ap)) return 1;
    std::cout << "\nЦепочка доверия построена, все компоненты аутентичны.\n";

    std::cout << "\nАтака 1: модификация приложения\n";
    ap.data[100] ^= 0xFF;
    if (!verify(ap)) std::cout << " Secure Boot остановил загрузку.\n";

    std::cout << "\nАтака 2: атакующий подменил и образ, и эталонный хеш\n";
    SecureImage forged{"app (подделка)", ap.data, sha256_of(ap.data)};
    if (verify(forged)) {
        std::cout << " Проверка пройдена: хеш без защиты эталона ничего не гарантирует.\n";
    }
    return 0;
}
