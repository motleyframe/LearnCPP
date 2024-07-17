#include <algorithm>
#include <cmath>
#include <iostream>
#include <map>
#include <numeric>
#include <stdexcept>
#include <string>

#include <tuple>
#include <vector>

using namespace std::string_literals;

// Перечислимый тип для статуса задачи
enum class TaskStatus {
    NEW,          // новая
    IN_PROGRESS,  // в разработке
    TESTING,      // на тестировании
    DONE          // завершена
};

// Объявляем тип-синоним для std::map<TaskStatus, int>,
// позволяющего хранить количество задач каждого статуса
using TasksInfo = std::map<TaskStatus, int>;

class TeamTasks {
private:
    const TasksInfo INIT {  {TaskStatus::NEW,0},
                            {TaskStatus::IN_PROGRESS,0},
                            {TaskStatus::TESTING,0},
                            {TaskStatus::DONE,0} };

    std::map<std::string, TasksInfo> person_tasks_;
    TasksInfo updated_;
    TasksInfo untouched_;

    static TaskStatus Next(const TaskStatus& current) {
        if(current != TaskStatus::DONE) {
            int status_value = static_cast<int>(current);
            return static_cast<TaskStatus>(++status_value);
        }
        return current;
    }

    int CalcPendingTasks(const std::string& person) const {
         const auto& t = person_tasks_.at(person);

         return std::accumulate(t.cbegin(),t.cend(),0,[]
                ( int ctr, const auto& task ) {
                    return task.first != TaskStatus::DONE ? ctr+task.second : ctr;
                } );
    }

    TasksInfo Diff(const TasksInfo& f, const TasksInfo& s) {
        TasksInfo delta;

        for(auto it=f.cbegin() ; it != f.cend() ; ++it) {
            auto& i = it->first;
            delta[i] = f.at(i)- s.at(i);
        }
        return delta;
    }

    TasksInfo Trunc(const TasksInfo& t, const bool trunc_done_status=false) {
        TasksInfo copy;
        for(auto it=t.cbegin(); it != t.cend(); ++it) {
            if( !(it->second == 0 || (trunc_done_status && it->first == TaskStatus::DONE)) )
                copy[it->first] = it->second;

        }
        return copy;
    }

public:
    TeamTasks() {
        updated_=INIT;
        untouched_=INIT;
    }
    // Получить статистику по статусам задач конкретного разработчика
    const TasksInfo& GetPersonTasksInfo(const std::string& person) const {
        return person_tasks_.at(person);
    }

    // Добавить новую задачу (в статусе NEW) для конкретного разработчика
    void AddNewTask(const std::string& person) {
        if(person_tasks_.find(person) == person_tasks_.end()) {
            person_tasks_[person] = INIT;
        } else
            ++person_tasks_[person][TaskStatus::NEW];
    }

    // Обновить статусы по данному количеству задач конкретного разработчика,
    // подробности см. ниже

    std::tuple<TasksInfo, TasksInfo> PerformPersonTasks(const std::string& person, int task_count) {
        //no such person
        if(person_tasks_.find(person) == person_tasks_.end()) {
            return std::make_tuple(TasksInfo(),TasksInfo());
        }

        auto& all = person_tasks_[person];
        const int pending_tasks = CalcPendingTasks(person);

        if(pending_tasks == 0 || task_count == 0) {
            untouched_ = all;
            updated_ = INIT;
            return std::make_tuple(Trunc(untouched_),Trunc(updated_));
        }

        task_count = task_count <= pending_tasks ? task_count : pending_tasks;

        for(auto it=all.begin() ; it != all.end() ; ++it) {
            const auto& status = it->first;

            if(status == TaskStatus::DONE || task_count == 0)
                break;

            auto& number_of_tasks = it->second;
            if(number_of_tasks==0) continue;

            const int delta = task_count >= number_of_tasks ? number_of_tasks
                                                            : task_count;
            updated_[Next(status)] = delta;
            task_count = task_count >= number_of_tasks ? task_count-number_of_tasks
                                                       : 0;
            all[status] = number_of_tasks - delta;
            all[Next(status)] = std::next(it)->second + delta;
        }

        try {
        untouched_ = Diff(all,updated_);
        } catch(std::out_of_range& e) {std::cerr<<e.what()<<"  DIFFERENCE\n"s<<std::endl;
            std::cout<<all.size()<<' '<<updated_.size()<<' '<<untouched_.size()<<std::endl;
        };

        return std::make_tuple(Trunc(updated_),Trunc(untouched_,true));
    }
};

// Принимаем словарь по значению, чтобы иметь возможность
// обращаться к отсутствующим ключам с помощью [] и получать 0,
// не меняя при этом исходный словарь.
void PrintTasksInfo(TasksInfo tasks_info) {
    std::cout << tasks_info[TaskStatus::NEW] << " new tasks"s
         << ", "s << tasks_info[TaskStatus::IN_PROGRESS] << " tasks in progress"s
         << ", "s << tasks_info[TaskStatus::TESTING] << " tasks are being tested"s
         << ", "s << tasks_info[TaskStatus::DONE] << " tasks are done"s << std::endl;
}

int main() {
    TeamTasks tasks;
    tasks.AddNewTask("Ilia"s);
    for (int i = 0; i < 3; ++i) {
        tasks.AddNewTask("Ivan"s);
    }
    std::cout << "Ilia's tasks: "s;
    PrintTasksInfo(tasks.GetPersonTasksInfo("Ilia"s));
    std::cout << "Ivan's tasks: "s;
    PrintTasksInfo(tasks.GetPersonTasksInfo("Ivan"s));

    TasksInfo updated_tasks, untouched_tasks;

    std::tie(updated_tasks, untouched_tasks) = tasks.PerformPersonTasks("Ivan"s, 2);

    std::cout << "Updated Ivan's tasks: "s;
    PrintTasksInfo(updated_tasks);
    std::cout << "Untouched Ivan's tasks: "s;
    PrintTasksInfo(untouched_tasks);

    std::tie(updated_tasks, untouched_tasks) = tasks.PerformPersonTasks("Ivan"s, 2);
    std::cout << "Updated Ivan's tasks: "s;
    PrintTasksInfo(updated_tasks);
    std::cout << "Untouched Ivan's tasks: "s;
    PrintTasksInfo(untouched_tasks);

    return EXIT_SUCCESS;
}
