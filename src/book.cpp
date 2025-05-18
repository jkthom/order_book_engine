#include "book.h"
#include <algorithm>




OrderBook::OrderBook()
  : pool_{}  // default pool using new_delete_resource
  // buys: comparator + pool allocator
  , buys(
      std::greater<double>{},
      std::pmr::polymorphic_allocator<
        std::pair<const double, PmrOrderList>
      >(&pool_)
    )
  , sells(
      std::greater<double>{},
      std::pmr::polymorphic_allocator<
        std::pair<const double, PmrOrderList>
      >(&pool_)
    )
  // order_index: bucket count, hasher, key_equal, pool allocator
  , order_index(
      2048,
      std::hash<uint64_t>{},
      std::equal_to<uint64_t>{},
      std::pmr::polymorphic_allocator<
        std::pair<
          const uint64_t,
          std::pair<double, PmrOrderList::iterator>
        >
      >(&pool_)
    )
{
    
}




std::vector<Trade> OrderBook::add_order(const Order& incoming) {
    std::vector<Trade> trades;
    Order o = incoming;  // mutable copy

    // BUY side: incoming Buy orders hit the lowest-price SELL book
    if (o.side == Side::Buy) {
        while (o.quantity > 0 && !sells.empty()) {
            auto it_level = sells.begin();           // best (lowest) sell price
            double best_price = it_level->first;

            // if it's a limit order, enforce price crossing
            if (o.price > 0 && best_price > o.price)
                break;

            auto& queue = it_level->second;
            auto maker_it = queue.begin();
            uint64_t executed = std::min(o.quantity, maker_it->quantity);

            trades.push_back(Trade{
                maker_it->id,
                o.id,
                maker_it->price,
                executed,
                std::max(o.timestamp, maker_it->timestamp)
            });

            o.quantity        -= executed;
            maker_it->quantity -= executed;

            if (maker_it->quantity == 0) {
                order_index.erase(maker_it->id);
                queue.erase(maker_it);
                if (queue.empty())
                    sells.erase(it_level);
            }
        }

        // If leftovers on a limit, add to BUY book
        if (o.quantity > 0 && o.price > 0) {
            auto& q = buys[o.price];
            q.push_back(o);
            order_index[o.id] = { o.price, std::prev(q.end()) };
        }
    }

    // SELL side: incoming Sell orders hit the highest-price BUY book
    else {
        while (o.quantity > 0 && !buys.empty()) {
            auto it_level = buys.begin();            // best (highest) buy price
            double best_price = it_level->first;

            if (o.price > 0 && best_price < o.price)
                break;

            auto& queue = it_level->second;
            auto maker_it = queue.begin();
            uint64_t executed = std::min(o.quantity, maker_it->quantity);

            trades.push_back(Trade{
                maker_it->id,
                o.id,
                maker_it->price,
                executed,
                std::max(o.timestamp, maker_it->timestamp)
            });

            o.quantity        -= executed;
            maker_it->quantity -= executed;

            if (maker_it->quantity == 0) {
                order_index.erase(maker_it->id);
                queue.erase(maker_it);
                if (queue.empty())
                    buys.erase(it_level);
            }
        }

        // If leftovers on a limit, add to SELL book
        if (o.quantity > 0 && o.price > 0) {
            auto& q = sells[o.price];
            q.push_back(o);
            order_index[o.id] = { o.price, std::prev(q.end()) };
        }
    }

    return trades;
}

bool OrderBook::cancel_order(uint64_t order_id) {
    auto idx_it = order_index.find(order_id);
    if (idx_it == order_index.end()) return false;

    double price          = idx_it->second.first;
    auto   list_it        = idx_it->second.second;
    Side   side_to_cancel = list_it->side;

    if (side_to_cancel == Side::Buy) {
        // remove from the BUY book
        auto map_it = buys.find(price);
        if (map_it != buys.end()) {
            map_it->second.erase(list_it);
            if (map_it->second.empty())
                buys.erase(map_it);
        }
    } else {
        // remove from the SELL book
        auto map_it = sells.find(price);
        if (map_it != sells.end()) {
            map_it->second.erase(list_it);
            if (map_it->second.empty())
                sells.erase(map_it);
        }
    }

    order_index.erase(idx_it);
    return true;
}

