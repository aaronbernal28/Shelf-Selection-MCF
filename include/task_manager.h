#ifndef TASK_MANAGER_H
#define TASK_MANAGER_H

#include "types.h"

namespace SS {

/**
 * @brief Manages task execution and pending tasks
 */
class TaskManager {
public:
    // Constructor
    TaskManager(int seed);

    // Process tasks in the taskpool
    Taskpool process_tasks(Taskpool &taskpool);

private:
    int seed_;
};

}

#endif // TASK_MANAGER_H
