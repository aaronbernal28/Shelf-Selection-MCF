#ifndef RACK_H
#define RACK_H

#include <string>
#include <vector>
#include <map>
#include "types.h"

namespace SS {
/**
 * @brief Represents a storage rack in the warehouse
 */
struct rack
{
    const RackID rack_id;
    bool is_warm = false; // Indicates if the rack is a warm rack
    bool is_hot = false; // Indicates if the rack is a hot rack
    std::map<FaceID, std::map<ItemID, int>> stock_faces; // FaceID -> (ItemID -> Quantity)
};

using racks = std::map<RackID, rack>;
}

#endif // RACK_H