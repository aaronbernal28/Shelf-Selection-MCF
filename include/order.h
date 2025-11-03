#ifndef ORDER_H
#define ORDER_H

#include <map>
#include "types.h"

namespace SS {

/**
 * @brief Represents an order in the shelf selection system
 */

// Status definitions
constexpr const char* PENDING = "Pending";
constexpr const char* PROCESSING = "Processing";
constexpr const char* COMPLETED = "Completed";
constexpr const char* EXPIRED = "Expired";

enum class OrderStatus { PENDING, PROCESSING, COMPLETED, EXPIRED };

// Order structure
struct Order
{
    const OrderID order_id;
    const ItemID item_id;
    const int quantity = 1;
    const TimePoint creation_date;
    const TimePoint due_date;
    TimePoint closure_date = TimePoint();
    OrderStatus status = OrderStatus::PENDING;
};

// Map of OrderID to Order
using Orders = std::map<OrderID, Order>;
}

#endif // ORDER_H
