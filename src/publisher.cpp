#include "../include/publisher.h"
#include <fstream>
#include <nlohmann/json.hpp>

namespace SS {

void Publisher::read_backlog_from_file() {
    // Implementation for reading the backlog from a file
    // For example json:
    /*
{
  "metadata": {
    "cpt_hours": [...]
  },
  "orders": [
    {
      "order_id": "ORD_013387_LXJY4YBS_000",
      "item_id": "LXJY4YBSWCX3KBF",
      "quantity": 1,
      "creation_date": "2025-10-09T00:00:04.885077",
      "due_date": "2025-10-09T21:00:00.885077"
    },
    {
      "order_id": "ORD_013387_LXJY4YBS_001",
      "item_id": "LXJY4YBSWCX3KBF",
      "quantity": 1,
      "creation_date": "2025-10-09T00:00:04.885077",
      "due_date": "2025-10-09T06:00:00.885077"
    }
    ]
}
    */
    std::ifstream file(backlog_file_path_);
    
    if (!file.is_open()) {
        throw std::runtime_error("Could not open backlog file: " + backlog_file_path_);
    }

    nlohmann::json json_data;
    file >> json_data;

    // Parse orders
    if (json_data.contains("orders")) {
        for (const auto &order_json : json_data["orders"]) {
            Order order;
            order.order_id = order_json["order_id"].get<std::string>();
            order.item_id = order_json["item_id"].get<std::string>();
            order.quantity = order_json["quantity"].get<int>();
            order.creation_date = order_json["creation_date"].get<std::string>();
            order.due_date = order_json["due_date"].get<std::string>();
            
            this->backlog_.push_back(order);
        }
    }
}

void Publisher::publish() {
    // Implementation for publishing an order to the database
    try {
        // Connect to the database
        pqxx::connection conn(build_connection_string());

        // Start a transaction
        pqxx::work txn(conn);

        // Calculate simulation time
        TimePoint simulation_date;

        for (const auto &order : backlog_) {
            auto elapsed_time = TimePoint::now() - this->simulation_start_date_;
            elapsed_time *= this->speed_up_factor_;
            simulation_date = this->start_date_ + elapsed_time;
            if (order.creation_date >= simulation_date) {
                // Insert order into database
                txn.exec_params(
                    "INSERT INTO orders (order_id, item_id, quantity, creation_date, due_date) "
                    "VALUES ($1, $2, $3, $4, $5)",
                    order.order_id,
                    order.item_id,
                    order.quantity,
                    order.creation_date,
                    order.due_date
                );
            } else {
                // Sleep for a short duration before checking again
                std::this_thread::sleep_for(std::chrono::milliseconds(1000 / this->speed_up_factor_));
            }
        }
        
        txn.commit(); // Commit the transaction
        conn.close(); // Close the connection

    } catch (const std::exception &e) {
        throw std::runtime_error("Failed to publish orders: " + std::string(e.what()));
    }
}

std::string Publisher::build_connection_string() const {
    return "dbname=" + this->dbname_ + 
           " user=" + this->db_user_ + 
           " host=" + this->db_host_ + 
           " port=" + this->db_port_;
}
}
