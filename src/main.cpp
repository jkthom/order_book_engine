#include "matching_engine.h"
#include <iostream>
#include <vector>
#include <random>
#include <chrono>

int main() {
    constexpr size_t N = 1'000'000;          // number of orders to simulate

    
    std::vector<Order> orders;
    orders.reserve(N);

    std::mt19937_64 rng(42);
    std::uniform_int_distribution<int> side_dist(0,1);
    std::uniform_int_distribution<int> price_tick(1, 1000);
    std::uniform_int_distribution<int> qty_dist(1, 100);

    uint64_t ts = 1;
    for (size_t i = 0; i < N; ++i) {
        Side side = side_dist(rng) ? Side::Buy : Side::Sell;
        double price = static_cast<double>(price_tick(rng));
        uint64_t qty = static_cast<uint64_t>(qty_dist(rng));
        // price==0 would be a market order; here all are limits
        orders.push_back(Order{ i+1, side, price, qty, ts++ });
    }

    //Run benchmark
    MatchingEngine engine;
    auto t0 = std::chrono::high_resolution_clock::now();
    for (auto &o : orders) {
        engine.process_new_order(o);
    }
    auto t1 = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> dt = t1 - t0;
    double secs = dt.count();
    double ops_per_sec = N / secs;

    std::cout << "Processed " << N << " orders in " << secs << " seconds\n"
              << "Throughput: " << ops_per_sec << " orders/sec\n";
    return 0;
}
