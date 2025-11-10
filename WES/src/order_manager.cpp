#include "order_manager.h"
#include "utils.h"
#include "stock.h"

namespace SS {

OrderManager::OrderManager(DBConnector& db_connector, StockManager& stock) 
    : db_connector_(db_connector), stock_(stock) {
}

std::vector<Order> OrderManager::get_backlog_from_db(pqxx::connection& conn, const TimePoint& simulation_date) {
    pqxx::work txn(conn);
    std::string sim_date_str = format_iso8601(simulation_date);
    pqxx::result result = txn.exec_params(
        "SELECT order_id, item_id, quantity, creation_date, due_date, "
        "CASE "
            "WHEN EXTRACT(EPOCH FROM ($1::timestamp - due_date))/60 <= 35 THEN 100 "
            "WHEN EXTRACT(EPOCH FROM ($1::timestamp - due_date))/60 <= 120 THEN 50 "
            "WHEN EXTRACT(EPOCH FROM ($1::timestamp - due_date))/60 <= 360 THEN 10 "
            "ELSE 1 "
        "END AS priority "
        "FROM backlog WHERE status = 'PENDING'",
        sim_date_str
    );
    
    std::vector<Order> backlog;
    backlog.reserve(result.size());
    
    for (const auto& row : result) {
        Order order{
            row["order_id"].as<std::string>(),
            row["item_id"].as<std::string>(),
            row["quantity"].as<int>(),
            parse_iso8601(row["creation_date"].as<std::string>()),
            parse_iso8601(row["due_date"].as<std::string>()),
            row["priority"].as<int>()
        };
        backlog.push_back(order);
    }
    
    txn.commit();
    return backlog;
}

void OrderManager::update_expired_orders(pqxx::connection& conn, const TimePoint& simulation_date) {
    pqxx::work txn(conn);
    std::string sim_date_str = format_iso8601(simulation_date);
    
    txn.exec_params(
        "UPDATE backlog SET status = 'EXPIRED', closure_date = $1 "
        "WHERE status = 'PENDING' AND due_date < $1",
        sim_date_str
    );
    
    txn.commit();
}

void OrderManager::update_stock_out_orders(pqxx::connection& conn) {
    // Implementation to update stock out orders in the database can be added here
    /*
    UPDATE backlog SET status = 'STOCK_OUT', closure_date = NOW()
    WHERE STATUS = 'PENDING' AND item_id IN (list_of_stock_out_item_ids);
    */
}

void OrderManager::update_completed_orders(pqxx::connection& conn, const Taskpool& taskpool) {
    pqxx::work txn(conn);
    TimePoint closure_time = std::chrono::system_clock::now();
    std::string closure_str = format_iso8601(closure_time);
    
    // Collect all order IDs from taskpool
    std::vector<std::string> order_ids;
    for (const auto& [rack_id, faces] : taskpool) {
        for (const auto& [face_id, orders] : faces) {
            order_ids.insert(order_ids.end(), orders.begin(), orders.end());
        }
    }
    
    // Update each order individually using parameterized queries to prevent SQL injection
    if (!order_ids.empty()) {
        for (const auto& order_id : order_ids) {
            txn.exec_params(
                "UPDATE backlog SET status = 'COMPLETED', closure_date = $1 WHERE order_id = $2",
                closure_str, order_id
            );
        }
    }
    
    txn.commit();
}

} // namespace SS
