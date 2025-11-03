#ifndef STOCK_H
#define STOCK_H

#include <map>
#include <string>
#include <vector>
#include "rack.h"

namespace SS {

/**
 * @brief Represents the stock of items in the shelf selection system
 */
class StockManager {
public:
    // Constructor
    StockManager(std::string stock_file_path) : stock_file_path_(stock_file_path) {}

    // Adds an item to the stock
    void add_item(const RackID &rack_id, const FaceID &face_id, const ItemID &item_id, int quantity);

    // Removes an item from the stock
    void remove_item(const RackID &rack_id, const FaceID &face_id, const ItemID &item_id, int quantity);

    // Checks the stock level of an item
    int check_stock(const RackID &rack_id, const FaceID &face_id, const ItemID &item_id) const;

    // Getters for the stock locations, racks, and faces
    std::vector<RackID> get_racks() const;
    std::vector<FaceID> get_faces() const;

private:
    // Map to hold the stock availability of items
    Stock stock_;

    // Map to hold the quantity of each item
    std::map<ItemID, int> item_quantities_;

    // Path to the stock file
    std::string stock_file_path_;

    // Vectors to hold the list of racks and faces
    Racks racks_;
    std::vector<FaceID> faces_;
};

}

#endif // STOCK_H