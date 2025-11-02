#ifndef PUBLISHER_H
#define PUBLISHER_H

#include <string>
#include <vector>
#include "types.h"
#include "order.h"

namespace SS {

/**
 * @brief Publishes orders from backlog based on simulation time. It publishes to a Postgres database.
 */
class Publisher {
public:
    // Constructor with optional speed-up factor
    Publisher(int speed_up_factor = 1, std::string stock_file_path) : speed_up_factor_(speed_up_factor), stock_file_path_(stock_file_path) {}

    // Add an order to the backlog
    void add_order_to_backlog(const Order &order);
    void read_backlog_from_file(const std::string &file_path);

    // Publish orders to the database
    int publish_orders(const std::string &db_connection_str);
    
private:
    int speed_up_factor_;
    std::string stock_file_path_;
    std::vector<Order> backlog_;
};

}

#endif // PUBLISHER_H
