#include "order.h"
#include "book.h"
#include <cassert>
#include <iostream>
#include <string>
#include <vector>
#include <utility>

// --- Helpers ------------------------------------------------

static uint64_t ts = 1;

Order make_limit(uint64_t id, Side s, double price, uint64_t qty) {
    return Order{id, s, price, qty, ts++};
}

// --- Test cases ---------------------------------------------

void test_order_struct() {
    Order o{1, Side::Buy, 123.45, 10, ts++};
    assert(o.id == 1);
    assert(o.side == Side::Buy);
    assert(o.price == 123.45);
    assert(o.quantity == 10);
}

void test_exact_fill() {
    OrderBook ob;
    ob.add_order(make_limit(1, Side::Sell, 100.0, 10));
    auto trades = ob.add_order(make_limit(2, Side::Buy, 100.0, 10));
    assert(trades.size() == 1);
    assert(trades[0].quantity == 10);
    assert(!ob.cancel_order(1));
}

void test_partial_fill() {
    OrderBook ob;
    ob.add_order(make_limit(1, Side::Sell, 100.0, 5));
    auto t1 = ob.add_order(make_limit(2, Side::Buy, 100.0, 10));
    assert(t1.size()==1 && t1[0].quantity == 5);
    auto t2 = ob.add_order(make_limit(3, Side::Sell, 100.0, 5));
    assert(t2.size()==1 && t2[0].quantity == 5);
}

void test_market_order() {
    OrderBook ob;
    ob.add_order(make_limit(1, Side::Sell, 50.0, 4));
    Order mkt{2, Side::Buy, 0.0, 4, ts++};
    auto trades = ob.add_order(mkt);
    assert(trades.size()==1);
    assert(trades[0].price == 50.0);
}

void test_cancel() {
    OrderBook ob;
    ob.add_order(make_limit(1, Side::Buy, 10.0, 1));
    assert(ob.cancel_order(1));
    assert(!ob.cancel_order(1));
}

// --- Test dispatcher ---------------------------------------

int main(int argc, char** argv) {
    using TestPair = std::pair<std::string, void(*)()>;
    std::vector<TestPair> tests = {
        {"order_struct",   test_order_struct},
        {"exact_fill",     test_exact_fill},
        {"partial_fill",   test_partial_fill},
        {"market_order",   test_market_order},
        {"cancel",         test_cancel}
    };

    if (argc == 1) {
        // Run them all
        for (auto &p : tests) p.second();
        std::cout<<"All tests passed\n";
        return 0;
    }

    std::string name = argv[1];
    for (auto &p : tests) {
        if (p.first == name) {
            p.second();
            std::cout<< name <<" passed\n";
            return 0;
        }
    }

    std::cerr<<"Unknown test: "<< name <<"\n";
    return 1;
}