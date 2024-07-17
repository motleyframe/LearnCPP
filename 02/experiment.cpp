#include <algorithm>
#include <iostream>
#include <iterator>
#include <numeric>
#include <map>

using namespace std::literals;

enum class TaskStatus {
    NEW,          // новая
    IN_PROGRESS,  // в разработке
    TESTING,      // на тестировании
    DONE          // завершена
};

using TasksInfo = std::map<TaskStatus, int>;

int Count1(const TasksInfo& t) {
    int counter=0;
    for(auto it=t.begin() ; it != t.end() ; ++it) {
        if(it != t.find(TaskStatus::NEW) )
            counter += std::next(it)->second;
    }

   /* return std::accumulate(t.cbegin(),t.cend(),0,[]
                ( int ctr, const auto& task ) {
                    return task.first != TaskStatus::TESTING ? ctr+task.second : ctr;
                } );*/
    return counter;
}

TasksInfo Diff(const TasksInfo& f, const TasksInfo& s) {
    TasksInfo delta;

    for(auto it=f.cbegin() ; it != f.cend() ; ++it) {
        auto& i = it->first;
        delta[i] = f.at(i)- s.at(i);
    }

    return delta;
}

const TasksInfo INIT { {TaskStatus::NEW,0},
                         {TaskStatus::IN_PROGRESS,0},
                         {TaskStatus::TESTING,0},
                         {TaskStatus::DONE,0} };




 void Trunc(TasksInfo& t) {
        for(auto it=t.begin(); it != t.end(); ) {
            if(it->first == TaskStatus::DONE || it->second == 0) {
                it = t.erase(it);
            } else ++it;
        }
 }

void PrintTasksInfo(TasksInfo tasks_info) {
    std::cout << tasks_info[TaskStatus::NEW] << " new tasks"s
         << ", "s << tasks_info[TaskStatus::IN_PROGRESS] << " tasks in progress"s
         << ", "s << tasks_info[TaskStatus::TESTING] << " tasks are being tested"s
         << ", "s << tasks_info[TaskStatus::DONE] << " tasks are done"s << std::endl;
}

int main() {
     const TasksInfo task1 = { {TaskStatus::NEW,10},
                        {TaskStatus::IN_PROGRESS,3},
                        {TaskStatus::TESTING,4},
                        {TaskStatus::DONE,10} };

//std::cout<<task1.at(TaskStatus::IN_PROGRESS)->second;
    //std::cout<<Count1(task1)<<std::endl;
    //std::cout<<Count()<<std::endl;

    const TasksInfo task2 = { {TaskStatus::NEW,5},
                        {TaskStatus::IN_PROGRESS,4},
                        {TaskStatus::TESTING,3},
                        {TaskStatus::DONE,2} };


    PrintTasksInfo(Diff(task1,task2));
    std::cout<<std::endl;

    return 0;
}
