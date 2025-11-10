#ifndef SHELF_SELECTOR_H
#define SHELF_SELECTOR_H

#include <vector>
#include "order.h"
#include "stock.h"
#include <set>

namespace SS {

/**
 * @brief Core shelf selection algorithm using MCF optimization
 */
class ShelfSelection {
public:
    // Constructor
    ShelfSelection(StockManager& stock): stock_(stock) {}
    
    // Main method
    Taskpool run(const std::vector<Order>& orders, Taskpool& pending, const int& N);

    // MCF
    Taskpool solve_mcf(const std::vector<Order>& orders, const int& limit);
    
private:
    // Member variables
    StockManager& stock_;
    std::vector<RackID> warm_racks_;
    std::set<RackID> hot_racks_;
    int warm_racks_limit;
};

}

#endif // SHELF_SELECTOR_H
