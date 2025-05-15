#pragma once
#include "order.h"
#include <vector>
#include <map>
#include <list>
#include <unordered_map>

struct Trade { /* define later */ };

class OrderBook {
public:
    std::vector<Trade> add_order(const Order&);
    bool cancel_order(uint64_t order_id);
private:
    std::map<double, std::list<Order>, std::greater<>> buys;
    std::map<double, std::list<Order>, std::less<>>    sells;
    std::unordered_map<uint64_t, 
      std::pair<double, std::list<Order>::iterator>> order_index;
};
