#pragma once
#include "book.h"

class MatchingEngine {
public:
    MatchingEngine();
    std::vector<Trade> process_new_order(const Order&);
    bool process_cancel(uint64_t order_id);
private:
    OrderBook book;
    uint64_t next_order_id = 1;
    uint64_t next_timestamp = 1;
    uint64_t gen_order_id()  { return next_order_id++; }
    uint64_t gen_timestamp() { return next_timestamp++; }
};
