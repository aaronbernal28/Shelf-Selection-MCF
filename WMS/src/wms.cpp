#include "publisher.h"
#include <fstream>
#include <thread>
#include <nlohmann/json.hpp>
#include <chrono>
#include <sstream>
#include <iomanip>

int main() {
    SS::TimePoint start_time = parse_iso8601_for_main("2025-10-09T00:00:00.000000");
    SS::TimePoint end_time = start_time + std::chrono::hours(24) + std::chrono::minutes(10);
    SS::TimePoint sim_start = std::chrono::system_clock::now();
    SS::Publisher publisher(1, start_time, end_time, sim_start, "data/raw/backlog.json");
    publisher.read_backlog_from_file();
    publisher.publish();
    return 0;
}