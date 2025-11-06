#include "publisher.h"
#include <fstream>
#include <thread>
#include <nlohmann/json.hpp>
#include <chrono>
#include <sstream>
#include <iomanip>

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
    file >> json_data;  // Read the JSON from the file
    
    // Parse orders
    if (json_data.contains("orders")) {
        for (const auto &order_json : json_data["orders"]) {
            std::string creation_date_str = order_json["creation_date"].get<std::string>();
            std::string due_date_str = order_json["due_date"].get<std::string>();
            
            TimePoint creation_date = parse_iso8601_date(creation_date_str);
            TimePoint due_date = parse_iso8601_date(due_date_str);
            
            Order order{
                order_json["order_id"].get<std::string>(),
                order_json["item_id"].get<std::string>(),
                order_json["quantity"].get<int>(),
                creation_date,
                due_date
            };
            
            this->backlog_.push_back(order);
        }
    }
}

// Helper function to convert TimePoint to ISO8601 string
std::string format_iso8601(const TimePoint& tp) {
    auto time = std::chrono::system_clock::to_time_t(tp);
    std::tm tm = *std::localtime(&time);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
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
            auto elapsed_time = std::chrono::system_clock::now() - this->simulation_start_date_;
            elapsed_time *= this->speed_up_factor_;
            simulation_date = this->start_date_ + elapsed_time;
            if (order.creation_date >= simulation_date) {
                // Convert timestamps to strings for database insertion
                std::string creation_date_str = format_iso8601(order.creation_date);
                std::string due_date_str = format_iso8601(order.due_date);
                
                // Insert order into database
                txn.exec_params(
                    "INSERT INTO backlog (order_id, item_id, quantity, creation_date, due_date) "
                    "VALUES ($1, $2, $3, $4, $5)",
                    order.order_id,
                    order.item_id,
                    order.quantity,
                    creation_date_str,
                    due_date_str
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

TimePoint Publisher::parse_iso8601_date(const std::string& date_str) const {
    std::tm tm = {};
    std::istringstream ss(date_str);
    ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%S");
    
    if (ss.fail()) {
        throw std::runtime_error("Failed to parse date string: " + date_str);
    }
    
    return std::chrono::system_clock::from_time_t(std::mktime(&tm));
}

}  // namespace SS

// Helper function for main (outside namespace)
SS::TimePoint parse_iso8601_for_main(const std::string& date_str) {
    std::tm tm = {};
    std::istringstream ss(date_str);
    ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%S");
    
    if (ss.fail()) {
        throw std::runtime_error("Failed to parse date string: " + date_str);
    }
    
    return std::chrono::system_clock::from_time_t(std::mktime(&tm));
}

