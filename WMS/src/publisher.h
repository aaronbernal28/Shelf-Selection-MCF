#ifndef PUBLISHER_H
#define PUBLISHER_H

#include <string>
#include <vector>
#include "order.h"
#include <pqxx/pqxx> // PostgreSQL C++ library
#include "types.h"
#include "db_connector.h"
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
        const std::string& backlog_file_path = "../data/raw/backlog.json");
    
    // Default constructor
    Publisher() : Publisher(1) {}
    
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
    DBConnector db_connector_;
};

}

#endif // PUBLISHER_H

