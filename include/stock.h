#ifndef STOCK_H
#define STOCK_H

#include "types.h"

namespace SS {

/**
 * @brief Represents the stock of items in the shelf selection system
 */
class Stock {
public:
    // Constructor
    Stock(std::string stock_file_path) : stock_file_path_(stock_file_path) {}

    // Adds an item to the stock
    void add_item(const Location &location, const ItemID &item_id, int quantity);

    // Removes an item from the stock
    void remove_item(const Location &location, const ItemID &item_id, int quantity);

    // Checks the stock level of an item
    int check_stock(const Location &location, const ItemID &item_id) const;

    // Getters for the stock locations, racks, and faces
    std::vector<Location> get_locations() const;
    std::vector<RackID> get_racks() const;
    std::vector<FaceID> get_faces() const;

private:
    // Map to hold the stock availability of items
    std::map<Location, std::map<ItemID, int>> stock_;

    // Map to hold the quantity of each item
    std::map<ItemID, int> item_quantities_;

    // Path to the stock file
    std::string stock_file_path_;

    std::vector<Location> locations_;
    std::vector<RackID> racks_;
    std::vector<FaceID> faces_;
};

}

#endif // STOCK_H