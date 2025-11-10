#ifndef ORDER_MANAGER_H
#define ORDER_MANAGER_H

#include <vector>
#include <pqxx/pqxx>
#include "types.h"
#include "order.h"
#include "db_connector.h"
#include "stock.h"

namespace SS {

/**
 * @brief Manages order database operations
 * Handles fetching, updating, and managing order statuses in the database
 */
class OrderManager {
public:
    // Constructor
    OrderManager(DBConnector& db_connector, StockManager& stock);

    // Fetch pending orders from database with status 'PENDING'
    std::vector<Order> get_backlog_from_db(pqxx::connection& conn, const TimePoint& simulation_date);

    // Update expired orders in the database
    void update_expired_orders(pqxx::connection& conn, const TimePoint& simulation_date);

    // Update stock out orders in the database
    void update_stock_out_orders(pqxx::connection& conn);

    // Update completed orders in the database
    void update_completed_orders(pqxx::connection& conn, const Taskpool& taskpool);

private:
    DBConnector& db_connector_;
    StockManager& stock_;
};

}

#endif // ORDER_MANAGER_H
