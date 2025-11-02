#ifndef TYPES_H
#define TYPES_H

#include <string>
#include <vector>
#include <chrono>
#include <map>

namespace SS {
// Type aliases for commonly used types in the shelf selection system
using RackID = std::string; // e.g., Rack_00001, Rack_00001, etc.
using FaceID = std::string; // Cara_1, Cara_2, Cara_3, Cara_4.
using ItemID = std::string; // e.g., 0N9X97RYEKKHWE1, LXJY4YBSWCX3KBF, etc.
using OrderID = std::string; // e.g., ORD_013387_LXJY4YBS_000, ORD_013387_LXJY4YBS_001, etc.
using TimePoint = std::chrono::system_clock::time_point;

// Define a Location as a pair of RackID and FaceID in the storage system
using Location = std::pair<RackID, FaceID>;

// Map from Location to list of OrderIDs assigned there
using Taskpool = std::map<Location, std::vector<OrderID>>;

// Status definitions
// Order Status
constexpr const char* PENDING = "Pending";
constexpr const char* IN_PROCESSES = "In_Processes";
constexpr const char* COMPLETED = "Completed";
constexpr const char* EXPIRED = "Expired";

enum class OrderStatus { PENDING, IN_PROCESSES, COMPLETED, EXPIRED };
} // namespace SS

#endif // TYPES_H