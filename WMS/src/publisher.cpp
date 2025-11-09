#include "publisher.h"
#include <fstream>
#include <thread>
#include <memory>
#include <nlohmann/json.hpp>
#include <chrono>
#include <iostream>
#include "utils.h"

namespace SS {

Publisher::Publisher(
    int speed_up_factor,
    TimePoint start_date,
    TimePoint end_date,
    TimePoint simulation_start_date,
    const std::string& backlog_file_path)
: speed_up_factor_(speed_up_factor), 
  start_date_(start_date),
  end_date_(end_date),
  simulation_start_date_(simulation_start_date),
  backlog_file_path_(backlog_file_path),
  db_connector_() {
}

void Publisher::read_backlog_from_file() {
    // Implementation for reading the backlog from a json file
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
            
            TimePoint creation_date = parse_iso8601(creation_date_str);
            TimePoint due_date = parse_iso8601(due_date_str);
            
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

void Publisher::publish() {
    // Implementation for publishing an order to the database
    try {
        std::cout << "Publisher starting with " << backlog_.size() << " orders in backlog" << std::endl;
        
        // Connect to the database using DBConnector
        pqxx::connection conn = db_connector_.connect();
        std::cout << "Database connected successfully" << std::endl;

        // Calculate simulation time
        TimePoint simulation_date;

        int published_count = 0;
        int batch_count = 0;
        const int BATCH_SIZE = 1000; // Commit every 1000 orders
        
        // Use a unique_ptr for transaction management
        std::unique_ptr<pqxx::work> txn = std::make_unique<pqxx::work>(conn);
        
        for (const auto &order : backlog_) {
            std::cout << "Checking order " << order.order_id << std::endl;
            while (true)
            {
            auto elapsed_time = std::chrono::system_clock::now() - this->simulation_start_date_;
            elapsed_time *= this->speed_up_factor_;
            simulation_date = this->start_date_ + elapsed_time;
            
            //std::cout << "  Order creation: " << format_iso8601(order.creation_date) << std::endl;
            //std::cout << "  Simulation date: " << format_iso8601(simulation_date) << std::endl;
            //std::cout << "  Condition (creation <= sim): " << (order.creation_date <= simulation_date) << std::endl;
            
            if (order.creation_date <= simulation_date) {
                // Convert timestamps to strings for database insertion
                std::string creation_date_str = format_iso8601(order.creation_date);
                std::string due_date_str = format_iso8601(order.due_date);
                
                // Insert order into database
                txn->exec(
                    "INSERT INTO backlog (order_id, item_id, quantity, creation_date, due_date) "
                    "VALUES ($1, $2, $3, $4, $5)",
                    pqxx::params(
                        order.order_id,
                        order.item_id,
                        order.quantity,
                        creation_date_str,
                        due_date_str
                    )
                );
                published_count++;
                batch_count++;
                //std::cout << "  -> Order published! (Total: " << published_count << ")" << std::endl;
                
                // Commit in batches to avoid losing all data on interruption
                if (batch_count >= BATCH_SIZE) {
                    txn->commit();
                    std::cout << "  >> Batch committed (" << published_count << " orders saved)" << std::endl;
                    txn = std::make_unique<pqxx::work>(conn); // Create new transaction
                    batch_count = 0;
                }
                
                break; // Exit the while loop to proceed to the next order
            } else {
                // Sleep for a short duration before checking again
                std::this_thread::sleep_for(std::chrono::milliseconds(1000 / this->speed_up_factor_));
            }
            }
        }
        
        // Commit any remaining orders in the last batch
        if (batch_count > 0) {
            txn->commit();
            std::cout << "  >> Final batch committed" << std::endl;
        }
        
        std::cout << "Published " << published_count << " orders total" << std::endl;
        conn.close(); // Close the connection

    } catch (const std::exception &e) {
        throw std::runtime_error("Failed to publish orders: " + std::string(e.what()));
    }
}

}  // namespace SS