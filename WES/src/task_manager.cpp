#include "task_manager.h"
#include <random>
#include <algorithm>
#include <vector>

namespace SS {

TaskManager::TaskManager(int seed) : seed_(seed) {}

Taskpool TaskManager::process_tasks(const Taskpool& taskpool) {
    /**
     * Processes the tasks selected by the Shelf Selector and returns pending tasks.
     * In practice, this procedure is not instantaneous.
     * 
     * Randomly selects K tasks (where K is random from 0-100) to be marked as pending
     * (tasks that could not be executed)
     */
    
    // Create random number generator with seed
    std::mt19937 rng(seed_);
    seed_ += 1; // Update seed for next call

    std::uniform_int_distribution<int> dist(0, 100);
    
    // Generate random K (number of pending tasks)
    int K = dist(rng);
    
    // Collect all keys (rack_id, face_id pairs) from taskpool
    std::vector<std::pair<RackID, FaceID>> keys;
    for (const auto& [rack_id, faces] : taskpool) {
        for (const auto& [face_id, orders] : faces) {
            keys.push_back({rack_id, face_id});
        }
    }
    
    // Calculate actual number of pending tasks (min of K and total keys)
    int num_pending = std::min(K, static_cast<int>(keys.size()));
    
    // Randomly sample keys for pending tasks
    std::shuffle(keys.begin(), keys.end(), rng);
    
    // Build pending taskpool with the first num_pending keys
    Taskpool pending_taskpool;
    for (int i = 0; i < num_pending; i++) {
        const auto& [rack_id, face_id] = keys[i];
        pending_taskpool[rack_id][face_id] = taskpool.at(rack_id).at(face_id);
    }
    
    return pending_taskpool;
}

} // namespace SS
