#ifndef TASK_MANAGER_H
#define TASK_MANAGER_H

#include "types.h"

namespace SS {

/**
 * @brief Manages task execution and pending tasks
 */
class TaskManager {
public:
    void process_tasks(Taskpool &taskpool);
    Taskpool get_pending_tasks() const;
    
private:
    int seed_;
    Taskpool last_taskpool_;
};

}

#endif // TASK_MANAGER_H
