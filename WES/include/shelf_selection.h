#ifndef SHELF_SELECTOR_H
#define SHELF_SELECTOR_H

#include <vector>
#include <deque>
#include <set>
#include "order.h"
#include "stock.h"

namespace SS {

/**
 * @brief Core shelf selection algorithm using MCF optimization
 */
class ShelfSelection {
public:
    // Constructor
    ShelfSelection(StockManager& stock);
    
    // Main method
    Taskpool run(const std::vector<Order>& orders, Taskpool& pending, const int& N);

    // MCF
    Taskpool solve_mcf(const std::vector<Order>& orders, const int& limit);
    
    // Helper methods
    void set_rack_warm(const RackID& rack_id);
    void reset_hot_racks();
    
private:
    // Member variables
    StockManager& stock_;
    std::deque<RackID> warm_racks_;
    std::set<RackID> hot_racks_;
    size_t warm_racks_limit;
};

}

#endif // SHELF_SELECTOR_H
