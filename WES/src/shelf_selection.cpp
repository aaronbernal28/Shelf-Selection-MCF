#include "shelf_selection.h"
#include <set>
#include <deque>
#include <cstddef>
#include <cstdint>
#include <vector>
#include <algorithm>
#include <map>
#include <string>
#include <stdexcept>
#include <cmath>
#include "ortools/graph/min_cost_flow.h"
#include "utils.h"

namespace SS {
// Constructor
ShelfSelection::ShelfSelection(StockManager& stock)
    : stock_(stock) {
    int rack_size = stock_.get_racks().size();
    warm_racks_limit = static_cast<size_t>(0.2 * rack_size);
    
    warm_racks_ = std::deque<RackID>{};
    hot_racks_ = std::set<RackID>{};
}

Taskpool ShelfSelection::run(const std::vector<Order>& orders, Taskpool& pending, const int& N) {
    // Implementation of the main shelf selection algorithm
    
    // Mark racks in pending as warm and count covered orders
    int covered_orders = 0;
    for (const auto& [rack_id, faces] : pending) {
        for (const auto& [face_id, orders] : faces) {
            covered_orders += orders.size();
            hot_racks_.insert(rack_id);
            stock_.is_rack_hot_[rack_id] = true;
        }
    }

    // Determine limit for MCF
    const int orders_size = orders.size();
    const int limit = std::min(N - covered_orders, orders_size);

    return solve_mcf(orders, limit);
}

Taskpool ShelfSelection::solve_mcf(const std::vector<Order>& orders, const int& limit) {
    // Implementation of the MCF optimization algorithm

    std::set<ItemID> items;
    std::vector<std::string> nodes_names = {};
    std::map<OrderID, ItemID> order_to_item; // Map to track which item each order needs
 
    // Generate a mapping of the nodes
    int node_index = 0;
    
    // Source node
    int source = node_index;
    node_index++;
    nodes_names.push_back("source");

    // Orders nodes
    std::map<OrderID, int> order_nodes;
    for (const auto& order : orders) {
        // Map order ID to node index
        order_nodes[order.order_id] = node_index;
        node_index++;
        nodes_names.push_back(order.order_id);
        items.insert(order.item_id);
        order_to_item[order.order_id] = order.item_id; // Store order-to-item mapping
    };

    // Rack_face nodes
    std::map<std::pair<RackID, FaceID>, int> rack_face_nodes;
    for (const auto& rack_id : stock_.get_racks()) {
        for (const auto& face_id : stock_.get_faces()) {
            // Map (rack_id, face_id) to node index
            rack_face_nodes[{rack_id, face_id}] = node_index;
            nodes_names.push_back(rack_id + " " + face_id);
            node_index++;
        }
    }

    // Items nodes
    std::map<ItemID, int> item_nodes;
    for (const auto& item_id : items) {
        // Map item ID to node index
        item_nodes[item_id] = node_index;
        nodes_names.push_back(item_id);
        node_index++;
    }

    // Sink node
    int sink = node_index;
    node_index++;
    nodes_names.push_back("sink");

    // Instantiate MinCostFlow solver
    operations_research::SimpleMinCostFlow min_cost_flow;
    
    // Add sink supplies
    min_cost_flow.SetNodeSupply(sink, limit);
    min_cost_flow.SetNodeSupply(source, -limit);

    // Source to order edges
    for (const auto& order : orders) {
        min_cost_flow.AddArcWithCapacityAndUnitCost(
            source, order_nodes[order.order_id], 1, -order.priority); // start, end, capacity, cost
    }
    
    std::vector<int> relevant_arcs = {};

    // Orders to rack_face edges
    for (const auto& order : orders) {
        for (const auto& rack_id : stock_.get_racks()) {
            for (const auto& face_id : stock_.get_faces()) {
                int arc = min_cost_flow.AddArcWithCapacityAndUnitCost(
                            order_nodes[order.order_id],
                            rack_face_nodes[{rack_id, face_id}],
                            1, 0); // start, end, capacity, cost
                relevant_arcs.push_back(arc);
            }
        }
    }

    // Rack_face to item edges
    for (const auto& rack_id : stock_.get_racks()) {
        for (const auto& face_id : stock_.get_faces()) {
            for (const auto& item_id : items) {
                int quantity = stock_.get_item_quantity(rack_id, face_id, item_id);
                int cost;
                if (stock_.is_rack_hot_[rack_id]) {
                    cost = -5;
                } else if (stock_.is_rack_warm_[rack_id]) {
                    cost = -3;
                } else {
                    cost = -1;
                }

                if (quantity > 0) {
                    min_cost_flow.AddArcWithCapacityAndUnitCost(
                        rack_face_nodes[{rack_id, face_id}],
                        item_nodes[item_id],
                        quantity, cost); // start, end, capacity, cost
                }
            }
        }
    }

    // Item to sink edges
    for (const auto& item_id : items) {
        min_cost_flow.AddArcWithCapacityAndUnitCost(
            item_nodes[item_id], sink, limit, 0); // start, end, capacity, cost
    }

    // Source to sink direct edge (for unfulfilled orders)
    min_cost_flow.AddArcWithCapacityAndUnitCost(
        source, sink, limit, 999999); // start, end, capacity, cost
    
    // Find the min cost flow.
    int status = min_cost_flow.Solve();

    if (status != operations_research::SimpleMinCostFlow::OPTIMAL) {
        throw std::runtime_error("Error: Solving the min cost flow problem failed.");
    }
    
    // Extract the solution into taskpool
    Taskpool taskpool;

    for (int arc : relevant_arcs) {
        if (min_cost_flow.Flow(arc) > 0.5) {
            int order_node = min_cost_flow.Tail(arc);
            int rack_face_node = min_cost_flow.Head(arc);

            // Find corresponding order ID in O(1)
            OrderID order_id = nodes_names[order_node];
            
            // Parse rack_id and face_id from the string "rack_id face_id"
            std::string rack_face_str = nodes_names[rack_face_node];
            size_t space_pos = rack_face_str.find(' ');
            RackID rack_id = rack_face_str.substr(0, space_pos);
            FaceID face_id = rack_face_str.substr(space_pos + 1);

            // Add to taskpool
            taskpool[rack_id][face_id].push_back(order_id);
            set_rack_warm(rack_id);

            // Update stock quantities
            ItemID item_id = order_to_item[order_id]; // Get the item_id for this order
            stock_.set_item_quantity(rack_id, face_id, item_id, -1);
        }
    }
    reset_hot_racks();
    return taskpool;
}

void ShelfSelection::set_rack_warm(const RackID& rack_id) {
    // Add rack to warm racks queue
    warm_racks_.push_back(rack_id);
    stock_.is_rack_warm_[rack_id] = true;

    if (warm_racks_.size() > warm_racks_limit) {
        // Remove the oldest warm rack
        RackID oldest_rack = warm_racks_.front();
        stock_.is_rack_warm_[oldest_rack] = false;
        warm_racks_.pop_front();
    }
}

void ShelfSelection::reset_hot_racks() {
    hot_racks_.clear();
    for (const auto& rack_id : stock_.get_racks()) {
        stock_.is_rack_hot_[rack_id] = false;
    }
}
} // namespace SS