#ifndef TASK_MANAGER_H
#define TASK_MANAGER_H

#include "types.h"
#include <vector>

namespace SS {

/**
 * @brief Manages task execution and pending tasks
 * Processes tasks selected by Shelf Selector and returns pending tasks
 * In practice, this process is not instantaneous
 */
class TaskManager {
public:
    // Constructor
    TaskManager(int seed = 28);

    // Process tasks from the taskpool, returns pending tasks
    Taskpool process_tasks(const Taskpool& taskpool);

    // Placeholder for available capacity retrieval
    int get_available_capacity();
private:
    int seed_;
};

}

#endif // TASK_MANAGER_H
