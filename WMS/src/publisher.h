#ifndef PUBLISHER_H
#define PUBLISHER_H

#include <string>
#include <vector>
#include "order.h"
#include <pqxx/pqxx> // PostgreSQL C++ library
#include "types.h"
#include <chrono>

namespace SS {

/**
 * @brief Publishes orders from backlog based on simulation time. It publishes to a Postgres database.
 */
class Publisher {
public:
    // Constructor with optional speed-up factor
    Publisher(
        int speed_up_factor = 1,
        TimePoint start_date = TimePoint(),
        TimePoint end_date = TimePoint(),
        TimePoint simulation_start_date = TimePoint(),
        const std::string& backlog_file_path = "../data/raw/backlog.json",
        const std::string& dbname = "my_MCF_db",
        const std::string& db_user = "aaron",
        const std::string& db_host = "localhost",
        const std::string& db_port = "5432")
    : speed_up_factor_(speed_up_factor), 
    start_date_(start_date),
    end_date_(end_date),
    simulation_start_date_(simulation_start_date),
    backlog_file_path_(backlog_file_path), 
    dbname_(dbname), 
    db_user_(db_user), 
    db_host_(db_host), 
    db_port_(db_port) {}
    
    // Generate backlog_ from a file
    void read_backlog_from_file();

    // Publish orders to the database
    void publish();

    // Parse ISO 8601 date string to TimePoint
    TimePoint parse_iso8601_date(const std::string& date_str) const;

    std::vector<Order> get_backlog() const { return backlog_; }
    
private:
    const int speed_up_factor_;
    TimePoint start_date_;
    TimePoint end_date_;
    TimePoint simulation_start_date_;
    std::vector<Order> backlog_;
    const std::string backlog_file_path_;
    const std::string dbname_;
    const std::string db_user_;
    const std::string db_host_;
    const std::string db_port_;
    
    // Helper method to build connection string
    std::string build_connection_string() const;
};

}

#endif // PUBLISHER_H

