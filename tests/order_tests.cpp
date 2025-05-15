#include "order.h"
#include <cassert>

int main() {
    Order o{1, Side::Buy, 100.0, 5, 1};
    assert(o.side == Side::Buy);
    assert(o.price == 100.0);
    return 0;
}
