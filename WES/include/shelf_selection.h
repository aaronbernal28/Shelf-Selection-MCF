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
    ShelfSelection(StockManager& stock): stock_(stock), rack_tibios_max_(0) {}
    
    // Main method
    Taskpool run(const std::vector<Order>& orders, Taskpool& pending, const int& N);

    // MCF
    Taskpool solve_mcf(const std::vector<Order>& orders, const int& limit);
    
private:
    // Member variables
    StockManager& stock_;
    int rack_tibios_max_;
};

}

#endif // SHELF_SELECTOR_H
