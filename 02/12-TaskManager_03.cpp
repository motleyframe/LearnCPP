#include <iostream>
#include <iostream>
#include <map>
#include <string>
#include <tuple>
#include <vector>

enum class TaskStatus {
    NEW,
    IN_PROGRESS,
    TESTING,
    DONE
};

using TaskInfo = std::map<TaskStatus,int>;




