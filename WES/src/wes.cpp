#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include "types.h"
#include "order.h"
#include "db_connector.h"
#include "stock.h"
#include "shelf_selection.h"
#include "task_manager.h"
#include "order_manager.h"
#include "utils.h"

int main() {
    try {
        std::cout << "Starting Shelf Selection Simulation..." << std::endl;
        
        // Configuration
        const int speed_up_factor = 1;
        SS::TimePoint start_time = SS::parse_iso8601("2025-10-09T00:00:00");
        SS::TimePoint end_time = start_time + std::chrono::hours(24) + std::chrono::minutes(10);
        SS::TimePoint sim_start = std::chrono::system_clock::now();
        const auto MINUTES_5 = std::chrono::minutes(5);
        
        // Initialize components
        SS::DBConnector db_connector;
        SS::StockManager stock("data/raw/stock.json");
        SS::ShelfSelection shelf_selector(stock);
        SS::TaskManager task_manager(28);
        SS::OrderManager order_manager(db_connector, stock);
        pqxx::connection conn = db_connector.connect();
        
        std::cout << "Database connected successfully" << std::endl;
        
        // Simulation variables
        SS::Taskpool pending;
        int iteration = 0;
        auto last_check = sim_start;
        
        while (true) {
            // Calculate elapsed time
            auto now = std::chrono::system_clock::now();
            auto elapsed_real = now - sim_start;
            auto elapsed_time = elapsed_real * speed_up_factor;
            SS::TimePoint simulation_date = start_time + elapsed_time;
            
            // Check if simulation duration reached
            if (simulation_date >= end_time) {
                std::cout << "\nSimulation duration reached. Exiting." << std::endl;
                break;
            }
            
            // Check if 5 minutes have passed
            auto time_since_check = now - last_check;
            if (time_since_check >= MINUTES_5 / speed_up_factor) {
                iteration++;
                last_check = now;
                
                std::cout << "\n🤖 Shelf Selector iteration " << iteration << " ===========================" << std::endl;
                auto elapsed_minutes = std::chrono::duration_cast<std::chrono::minutes>(elapsed_time).count();
                std::cout << "  ├─ Current time: " << elapsed_minutes << " minutes" << std::endl;
                
                // Update expired orders in DB
                order_manager.update_expired_orders(conn, simulation_date);
                
                // Fetch pending backlog from DB
                std::vector<SS::Order> backlog = order_manager.get_backlog_from_db(conn);
                std::cout << "  ├─ Pending orders: " << backlog.size() << std::endl;
                
                // Run shelf selector to get taskpool
                int N = task_manager.get_available_capacity();
                SS::Taskpool taskpool = shelf_selector.run(backlog, pending, N);
                
                // Update DB with completed tasks
                order_manager.update_completed_orders(conn, taskpool);
                
                // Process tasks and get pending tasks
                pending = task_manager.process_tasks(taskpool);
                
                std::cout << "  └─ Next pending tasks: " << pending.size() << std::endl;
            } else {
                // Sleep for a short duration before checking again
                std::this_thread::sleep_for(std::chrono::milliseconds(1000 / speed_up_factor));
            }
        }

        conn.close();
        std::cout << "Simulation completed successfully." << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}