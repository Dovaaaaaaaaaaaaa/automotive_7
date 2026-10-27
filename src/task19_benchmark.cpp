#include <chrono>
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <random>
#include "adler32.hpp"
#include "crc16.hpp"
#include "crc32.hpp"
#include "crc8_bitwise.hpp"
#include "crc8_table.hpp"
#include "djb2.hpp"
#include "fletcher16.hpp"
#include "fnv.hpp"
#include "murmur3.hpp"
#include "sha256.hpp"
#include "sum_mod256.hpp"
#include "xor_sum.hpp"

// Возвращает среднее время одного вызова f(data) в микросекундах
template <typename F>
double bench(F f, const Bytes& data, int reps) {
    volatile std::uint64_t sink = 0; // чтобы компилятор не выбросил вычисления
    auto t0 = std::chrono::steady_clock::now();
    for (int i = 0; i < reps; ++i) sink = sink + f(data);
    auto t1 = std::chrono::steady_clock::now();
    return std::chrono::duration<double, std::micro>(t1 - t0).count() / reps;
}

int main() {
    Bytes data(1024 * 1024); // 1 МБ случайных данных
    std::mt19937 rng(42);
    for (auto& b : data) b = static_cast<std::uint8_t>(rng() & 0xFF);
    
    const int REPS = 20;
    std::cout << "Размер данных: " << data.size() << " байт, повторов: " << REPS << "\n\n";
    std::cout << "Алгоритм              | мкс      | МБ/с\n";
    std::cout << "----------------------+----------+--------\n";

    auto print = [&](const char* name, double us) {
        double mbps = static_cast<double>(data.size()) / (us * 1e-6) / (1024.0 * 1024.0);
        std::printf("%-21s | %8.1f | %7.1f\n", name, us, mbps);
    };

    print("XOR", bench([](const Bytes& d){ return std::uint64_t{xor_checksum(d)}; }, data, REPS));
    print("Sum mod 256", bench([](const Bytes& d){ return std::uint64_t{sum_mod256(d)}; }, data, REPS));
    print("Fletcher-16", bench([](const Bytes& d){ return std::uint64_t{fletcher16(d)}; }, data, REPS));
    print("Adler-32", bench([](const Bytes& d){ return std::uint64_t{adler32(d)}; }, data, REPS));
    print("CRC-8 bitwise", bench([](const Bytes& d){ return std::uint64_t{crc8_sae_j1850(d)}; }, data, REPS));
    print("CRC-8 table", bench([](const Bytes& d){ return std::uint64_t{Crc8SaeJ1850{}.compute(d)}; }, data, REPS));
    print("CRC-16 table", bench([](const Bytes& d){ return std::uint64_t{Crc16CcittFalse{}.compute(d)}; }, data, REPS));
    print("CRC-32 table", bench([](const Bytes& d){ return std::uint64_t{Crc32IsoHdlc{}.compute(d)}; }, data, REPS));
    print("FNV-1a 32", bench([](const Bytes& d){ return std::uint64_t{fnv1a_32(d.data(), d.size())}; }, data, REPS));
    print("DJB2", bench([](const Bytes& d){ return std::uint64_t{djb2(d.data(), d.size())}; }, data, REPS));
    print("MurmurHash3", bench([](const Bytes& d){ return std::uint64_t{murmur3_32(d.data(), d.size())}; }, data, REPS));
    
    print("SHA-256", bench([](const Bytes& d) {
        SHA256 s;
        s.update(d.data(), d.size());
        auto h = s.digest();
        return std::uint64_t{h[0]};
    }, data, 5));
    
    return 0;
}
