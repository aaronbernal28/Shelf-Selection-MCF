#ifndef STOCK_H
#define STOCK_H

#include <map>
#include <string>
#include <vector>
#include <nlohmann/json_fwd.hpp>
#include "rack.h"

namespace SS {

/**
 * @brief Represents the stock of items in the shelf selection system
 */
class StockManager {
public:
    // Constructor - loads stock from JSON file
    StockManager(const std::string& stock_file_path);

    // Load and process stock from JSON file
    void load_stock();

    // Get/Set item quantity at specific location (rack, face, item)
    int get_item_quantity(const RackID& rack_id, const FaceID& face_id, const ItemID& item_id) const;
    void set_item_quantity(const RackID& rack_id, const FaceID& face_id, const ItemID& item_id, int quantity);

    // Get total quantity of an item across all locations
    int get_total_quantity(const ItemID& item_id) const;

    // Get all items at a specific rack and face
    std::vector<ItemID> get_items(const RackID& rack_id, const FaceID& face_id) const;

    // Get all rack IDs in inventory
    std::vector<RackID> get_racks() const;

    // Get the entire stock structure
    const Stock& get_inventory() const { return inventory_; }

    // Stock out items
    std::vector<ItemID> stock_out_items_;

private:
    // Nested map structure: rack -> face -> item -> quantity
    Stock inventory_;

    // Map to hold total quantity of each item across all locations
    std::map<ItemID, int> items_quantity_;

    // Path to the stock JSON file
    std::string stock_file_path_;

    // Helper to process JSON and populate inventory
    void process_stock_json(const nlohmann::json& json_data);
};

}

#endif // STOCK_H