#pragma once

#include "order.h"

#include <map>
#include <list>
#include <vector>
#include <unordered_map>
#include <memory_resource>  
#include <functional>       
#include <cstdint>

// A trade record for a matched order pair
struct Trade {
    uint64_t maker_id;   // resting order’s id
    uint64_t taker_id;   // incoming order’s id
    double   price;      // execution price (resting order’s price)
    uint64_t quantity;   // how many shares traded
    uint64_t timestamp;  // when the trade happened
};

class OrderBook {
public:
    OrderBook();

    // Submit a new order (limit if price>0, otherwise market). Returns all trades.
    std::vector<Trade> add_order(const Order& incoming);

    // Cancel a resting order by ID. Returns true if found and removed.
    bool cancel_order(uint64_t order_id);

private:
    
    // 1) A single monotonic bump‐allocator for all our nodes
    std::pmr::monotonic_buffer_resource pool_;

    
    // 2) A list of Orders that draws its node storage from pool_
    using PmrOrderList = std::list<
        Order,
        std::pmr::polymorphic_allocator<Order>
    >;

    
    // 3) A price→order‐queue map that uses pool_ for its internal nodes
    //    - Key: price
    //    - Value: FIFO list of orders at that price
    //    - Comparator: greater<double> so highest‐price-first for buys
    //    - Allocator: from pool_
    using PriceLevels = std::map<
        double,
        PmrOrderList,
        std::greater<double>,
        std::pmr::polymorphic_allocator<std::pair<const double, PmrOrderList>>
    >;

    PriceLevels buys;   // buy‐side: price to order list (highest price first)
    PriceLevels sells;  // sell‐side: price to order list (highest price first)

    
    // 4) An index to find & cancel orders quickly:
    //    order_id to (price level, iterator into that PmrOrderList)
    using OrderIndex = std::pmr::unordered_map<
        uint64_t,                            // key: order id
        std::pair<double, PmrOrderList::iterator>, // value: price + position
        std::hash<uint64_t>,                 // hash functor
        std::equal_to<uint64_t>              // key equality
    >;

    OrderIndex order_index;
};
