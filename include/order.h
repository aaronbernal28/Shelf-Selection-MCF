#ifndef ORDER_H
#define ORDER_H

#include "types.h"

namespace SS {

/**
 * @brief Represents an order in the shelf selection system
 */
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

using Orders = std::map<OrderID, Order>;

}

#endif // ORDER_H
