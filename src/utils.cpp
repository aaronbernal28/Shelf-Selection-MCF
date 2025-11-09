#include "utils.h"
#include <sstream>
#include <iomanip>
#include <chrono>

namespace SS {

TimePoint parse_iso8601(const std::string& date_str) {
    std::tm tm = {};
    std::istringstream ss(date_str);
    ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%S");
    
    if (ss.fail()) {
        throw std::runtime_error("Failed to parse date string: " + date_str);
    }
    
    return std::chrono::system_clock::from_time_t(std::mktime(&tm));
}

std::string format_iso8601(const TimePoint& tp) {
    auto time = std::chrono::system_clock::to_time_t(tp);
    std::tm tm = *std::localtime(&time);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

}
