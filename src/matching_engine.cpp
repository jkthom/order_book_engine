#include "matching_engine.h"

MatchingEngine::MatchingEngine() = default;

std::vector<Trade> MatchingEngine::process_new_order(const Order& o) {
    // forward to the book
    return book.add_order(o);
}

bool MatchingEngine::process_cancel(uint64_t order_id) {
    return book.cancel_order(order_id);
}
