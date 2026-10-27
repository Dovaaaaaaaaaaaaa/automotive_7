#include <chrono>
#include <iostream>
#include <random>
#include "crc8_bitwise.hpp"
#include "crc8_table.hpp"

int main() {
    Bytes big(1024 * 1024); // 1 МБ
    std::mt19937 rng(42);
    for (auto& b : big) b = static_cast<std::uint8_t>(rng() & 0xFF);

    Crc8SaeJ1850 table;
    auto t0 = std::chrono::steady_clock::now();
    auto r1 = table.compute(big);
    auto t1 = std::chrono::steady_clock::now();
    
    auto r2 = crc8_sae_j1850(big);
    auto t2 = std::chrono::steady_clock::now();

    auto us_table = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();
    auto us_bit = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();

    std::cout << "CRC-8 табличный: " << us_table << " мкс, результат " << hex_str(r1, 2) << "\n";
    std::cout << "CRC-8 побитовый: " << us_bit << " мкс, результат " << hex_str(r2, 2) << "\n";
    std::cout << "Результаты совпадают: " << (r1 == r2 ? "ДА" : "НЕТ") << "\n";
    std::cout << "Ускорение: " << static_cast<double>(us_bit) / static_cast<double>(us_table) << "x\n";
    return 0;
}
