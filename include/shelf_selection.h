#ifndef SHELF_SELECTOR_H
#define SHELF_SELECTOR_H

#include <string>
#include <vector>
#include <map>
#include "order.h"
#include "types.h"
#include "stock.h"

namespace SS {

/**
 * @brief Core shelf selection algorithm using MCF optimization
 */
class ShelfSelection {
public:
    // Constructor
    ShelfSelection(Stock& stock, TimePoint stat_time): stock_(stock), stat_time_(stat_time) {}
    
    // Main method
    Taskpool run(const std::vector<Order>& orders);
    
private:
    // Member variables
    Stock& stock_;
    TimePoint stat_time_;
    std::map<RackID, RackStatus> rack_status_map_;
    std::vector<RackID> rack_tibios_;
    int rack_tibios_max_;

};

}

#endif // SHELF_SELECTOR_H
