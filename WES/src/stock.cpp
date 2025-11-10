#include "stock.h"
#include <fstream>
#include <stdexcept>
#include <nlohmann/json.hpp>

namespace SS {

StockManager::StockManager(const std::string& stock_file_path) 
    : stock_file_path_(stock_file_path) {
    load_stock();
    stock_out_items_ = std::vector<ItemID>();
    racks_ = std::set<RackID>();

    for (const auto& [rack_id, _] : inventory_) {
        racks_.insert(rack_id);
    }

    faces_ = std::set<FaceID>{"Cara_1", "Cara_2", "Cara_3", "Cara_4"};

    is_rack_hot_ = std::map<RackID, bool>();
    is_rack_warm_ = std::map<RackID, bool>();
    // Initialize rack status maps as false
    for (const auto& rack_id : racks_) {
        is_rack_hot_[rack_id] = false;
        is_rack_warm_[rack_id] = false;
    }
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
    // Update the inventory
    inventory_[rack_id][face_id][item_id] += quantity;
    
    // Update total quantity
    items_quantity_[item_id] += quantity;

    if (items_quantity_[item_id] <= 0) {
        // If total quantity is zero or negative, mark as stock out
        stock_out_items_.push_back(item_id);
    }
}

int StockManager::get_total_quantity(const ItemID& item_id) const {
    auto it = items_quantity_.find(item_id);
    if (it != items_quantity_.end()) {
        return it->second;
    }
    return 0;
}

} // namespace SS