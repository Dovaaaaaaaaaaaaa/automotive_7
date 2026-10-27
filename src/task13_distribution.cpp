#include <cstdint>
#include <iostream>
#include <random>
#include <set>
#include <string>
#include <vector>
#include "djb2.hpp"
#include "fnv.hpp"
#include "murmur3.hpp"

constexpr int BUCKETS = 1024;

template <typename Hash>
void analyze(const char* name, Hash h, const std::vector<std::string>& keys) {
    std::vector<int> load(BUCKETS, 0);
    std::set<std::uint32_t> unique_hashes;
    
    for (const auto& k : keys) {
        std::uint32_t v = h(k.data(), k.size());
        unique_hashes.insert(v);
        load[v % BUCKETS]++;
    }
    
    int max_load = 0, empty = 0;
    double expected = static_cast<double>(keys.size()) / BUCKETS;
    double chi2 = 0.0;
    
    for (int n : load) {
        if (n > max_load) max_load = n;
        if (n == 0) ++empty;
        double d = n - expected;
        chi2 += d * d / expected;
    }
    
    std::cout << name << ":\n"
              << " Ключей: " << keys.size() << "\n"
              << " Разных значений хеша: " << unique_hashes.size() << "\n"
              << " Коллизий 32-битного хеша: " << keys.size() - unique_hashes.size() << "\n"
              << " Пустых бакетов из " << BUCKETS << ": " << empty << "\n"
              << " Самый заполненный бакет: " << max_load
              << " (в среднем " << expected << ")\n"
              << " Хи-квадрат: " << chi2 << " (при идеальном равномерном ~" << BUCKETS - 1 << ")\n\n";
}

void run_all(const char* title, const std::vector<std::string>& keys) {
    std::cout << "=== " << title << " ===\n";
    analyze("FNV-1a 32", [](const void* p, std::size_t n) { return fnv1a_32(p, n); }, keys);
    analyze("DJB2", [](const void* p, std::size_t n) { return djb2(p, n); }, keys);
    analyze("Murmur3", [](const void* p, std::size_t n) { return murmur3_32(p, n); }, keys);
    // Заведомо плохой хеш: только первый байт ключа
    analyze("FirstByteOnly",
        [](const void* p, std::size_t) {
            return static_cast<std::uint32_t>(*static_cast<const std::uint8_t*>(p));
        },
        keys);
}

int main() {
    // Набор 1: случайные строки из 16 строчных латинских букв
    std::vector<std::string> random_keys;
    std::mt19937 rng(12345);
    for (int i = 0; i < 10000; ++i) {
        std::string k(16, 'a');
        for (auto& c : k) c = static_cast<char>('a' + rng() % 26);
        random_keys.push_back(k);
    }
    run_all("Случайные строки (16 символов)", random_keys);

    // Набор 2: «реалистичные» имена сигналов: Sig_0000 ... Sig_9999
    std::vector<std::string> signal_keys;
    for (int i = 0; i < 10000; ++i) {
        std::string num = std::to_string(i);
        signal_keys.push_back("Sig_" + std::string(4 - num.size(), '0') + num);
    }
    run_all("Имена сигналов Sig_0000 ... Sig_9999", signal_keys);
    
    return 0;
}
