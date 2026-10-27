#include <algorithm>
#include <cstdint>
#include <functional>
#include <iostream>
#include <string>
#include <unordered_map>

struct CanSignal {
    std::uint16_t message_id;
    std::uint8_t start_bit;
    std::uint8_t length;
    
    bool operator==(const CanSignal& o) const {
        return message_id == o.message_id && start_bit == o.start_bit && length == o.length;
    }
};

// Плохо: комбинирование через XOR (симметрично, hash(x, x) = 0)
struct BadHash {
    std::size_t operator()(const CanSignal& s) const {
        return std::hash<std::uint16_t>{}(s.message_id)
             ^ std::hash<std::uint8_t>{}(s.start_bit)
             ^ std::hash<std::uint8_t>{}(s.length);
    }
};

// Хорошо: комбинирование через «золотое сечение» (аналог boost::hash_combine)
struct GoodHash {
    std::size_t operator()(const CanSignal& s) const {
        std::size_t seed = 0;
        auto mix = [&seed](std::size_t h) {
            seed ^= h + 0x9e3779b97f4a7c15ULL + (seed << 6) + (seed >> 2);
        };
        mix(std::hash<std::uint16_t>{}(s.message_id));
        mix(std::hash<std::uint8_t>{}(s.start_bit));
        mix(std::hash<std::uint8_t>{}(s.length));
        return seed;
    }
};

int main() {
    BadHash bad;
    GoodHash good;
    
    CanSignal s1{0x100, 8, 16};
    CanSignal s2{0x100, 16, 8}; // те же числа, но start_bit и length поменяны местами
    CanSignal s3{0x20, 0x20, 0}; // message_id == start_bit
    
    std::cout << "BadHash(s1) = " << bad(s1) << "\n";
    std::cout << "BadHash(s2) = " << bad(s2) << "\n";
    std::cout << "Разные сигналы, одинаковый хеш: "
              << (bad(s1) == bad(s2) ? "ДА (плохо)" : "НЕТ") << "\n";
    std::cout << "BadHash(s3) = " << bad(s3) << " (равные поля обнулили друг друга)\n";
    
    std::cout << "\nGoodHash(s1) = " << good(s1) << "\n";
    std::cout << "GoodHash(s2) = " << good(s2) << "\n";
    std::cout << "Разные сигналы, одинаковый хеш: "
              << (good(s1) == good(s2) ? "ДА (плохо)" : "НЕТ (хорошо)") << "\n";
              
    // Теперь используем хеши в настоящих контейнерах
    std::unordered_map<CanSignal, std::string, GoodHash> map;
    map[s1] = "Engine_Torque";
    map[s2] = "Engine_Speed";
    map[s3] = "Vehicle_Speed";
    
    std::cout << "\nРазмер карты = " << map.size() << "\n";
    std::cout << "Найден s2: " << map.at(s2) << "\n";
    std::cout << "bucket_count = " << map.bucket_count() << "\n";
    std::cout << "load_factor = " << map.load_factor() << "\n";
    
    // Худший случай: BadHash склеивает много разных ключей в одну цепочку
    std::unordered_map<CanSignal, int, BadHash> bad_map;
    std::unordered_map<CanSignal, int, GoodHash> good_map;
    
    for (int a = 0; a < 64; ++a) {
        for (int b = 0; b < 64; ++b) {
            CanSignal key{0x100, static_cast<std::uint8_t>(a), static_cast<std::uint8_t>(b)};
            bad_map[key] = 1;
            good_map[key] = 1;
        }
    }
    
    auto longest = [](const auto& m) {
        std::size_t mx = 0;
        for (std::size_t i = 0; i < m.bucket_count(); ++i)
            mx = std::max(mx, m.bucket_size(i));
        return mx;
    };
    
    std::cout << "\n4096 ключей, самая длинная цепочка в бакете:\n";
    std::cout << " BadHash : " << longest(bad_map) << "\n";
    std::cout << " GoodHash: " << longest(good_map) << "\n";
    return 0;
}
