#ifndef SHELF_SELECTOR_H
#define SHELF_SELECTOR_H

#include <vector>
#include "order.h"
#include "stock.h"

namespace SS {

/**
 * @brief Core shelf selection algorithm using MCF optimization
 */
class ShelfSelection {
public:
    // Constructor
    ShelfSelection(StockManager& stock, TimePoint stat_time): stock_(stock), stat_time_(stat_time), rack_tibios_max_(0) {}
    
    // Main method
    Taskpool run(const std::vector<Order>& orders);
    
private:
    // Member variables
    StockManager& stock_;
    TimePoint stat_time_;
    int rack_tibios_max_;
};

}

#endif // SHELF_SELECTOR_H
