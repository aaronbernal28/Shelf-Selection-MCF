#ifndef UTILS_H
#define UTILS_H

#include <string>
#include "types.h"

namespace SS {

//Parse ISO8601 date string to TimePoint
TimePoint parse_iso8601(const std::string& date_str);

// Format TimePoint to ISO8601 date string
std::string format_iso8601(const TimePoint& tp);

}

#endif // UTILS_H
