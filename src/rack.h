#ifndef RACK_H
#define RACK_H

#include <map>
#include "types.h"

namespace SS {
/**
 * @brief Represents a storage rack in the warehouse
 */
struct Rack
{
    bool is_warm = false; // Indicates if the rack is a warm rack
    bool is_hot = false; // Indicates if the rack is a hot rack
};

using Racks = std::map<RackID, Rack>;
}

#endif // RACK_H