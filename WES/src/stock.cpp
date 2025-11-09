#include "stock.h"
#include <fstream>
#include <stdexcept>
#include <nlohmann/json.hpp>

namespace SS {

StockManager::StockManager(const std::string& stock_file_path) 
    : stock_file_path_(stock_file_path) {
    load_stock();
    stock_out_items_ = std::vector<ItemID>();
}

void StockManager::load_stock() {
    std::ifstream file(stock_file_path_);
    
    if (!file.is_open()) {
        throw std::runtime_error("Could not open stock file: " + stock_file_path_);
    }

    nlohmann::json json_data;
    file >> json_data;
    
    process_stock_json(json_data);
}

void StockManager::process_stock_json(const nlohmann::json& json_data) {
    // Clear existing data
    inventory_.clear();
    items_quantity_.clear();
    
    // Iterate through racks (top level keys)
    for (auto& [rack_id, rack_data] : json_data.items()) {
        inventory_[rack_id] = std::map<FaceID, std::map<ItemID, int>>();
        
        // Iterate through faces
        for (auto& [face_id, face_data] : rack_data.items()) {
            inventory_[rack_id][face_id] = std::map<ItemID, int>();
            
            // Check if face_data is an array (not null/empty)
            if (face_data.is_array()) {
                // Process each item in the face
                for (const auto& item : face_data) {
                    std::string item_id = item["Inventory ID"].get<std::string>();
                    int cantidad = item["Cantidad"].get<int>();
                    
                    // Aggregate quantities for same item (group by Inventory ID)
                    if (inventory_[rack_id][face_id].find(item_id) == inventory_[rack_id][face_id].end()) {
                        inventory_[rack_id][face_id][item_id] = 0;
                    }
                    inventory_[rack_id][face_id][item_id] += cantidad;
                    
                    // Update total quantity for this item
                    if (items_quantity_.find(item_id) == items_quantity_.end()) {
                        items_quantity_[item_id] = 0;
                    }
                    items_quantity_[item_id] += cantidad;
                }
            }
        }
    }
}

int StockManager::get_item_quantity(const RackID& rack_id, const FaceID& face_id, const ItemID& item_id) const {
    try {
        return inventory_.at(rack_id).at(face_id).at(item_id);
    } catch (const std::out_of_range&) {
        return 0; // Return 0 if not found
    }
}

void StockManager::set_item_quantity(const RackID& rack_id, const FaceID& face_id, const ItemID& item_id, int quantity) {
    int old_value = get_item_quantity(rack_id, face_id, item_id);
    
    // Update the inventory
    inventory_[rack_id][face_id][item_id] += quantity;
    
    // Update total quantity
    items_quantity_[item_id] += (quantity - old_value);
}

int StockManager::get_total_quantity(const ItemID& item_id) const {
    auto it = items_quantity_.find(item_id);
    if (it != items_quantity_.end()) {
        return it->second;
    }
    return 0;
}

std::vector<ItemID> StockManager::get_items(const RackID& rack_id, const FaceID& face_id) const {
    std::vector<ItemID> items;
    
    try {
        const auto& face_items = inventory_.at(rack_id).at(face_id);
        items.reserve(face_items.size());
        for (const auto& [item_id, quantity] : face_items) {
            items.push_back(item_id);
        }
    } catch (const std::out_of_range&) {
        // Return empty vector if rack/face not found
    }
    
    return items;
}

std::vector<RackID> StockManager::get_racks() const {
    std::vector<RackID> racks;
    racks.reserve(inventory_.size());
    
    for (const auto& [rack_id, rack_data] : inventory_) {
        racks.push_back(rack_id);
    }
    
    return racks;
}

} // namespace SS