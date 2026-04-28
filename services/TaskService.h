#pragma once

#include "../models/Task.h"
#include "../repositories/TaskRepository.h"

#include <optional>
#include <string>
#include <vector>

class TaskService {
public:
    explicit TaskService(TaskRepository repository);

    Task CreateTask(
        const std::string& title,
        const std::string& description,
        const std::string& status,
        const std::string& priority
    );

    std::vector<Task> GetAllTasks(
        const std::string& status,
        const std::string& priority,
        const std::string& search,
        const std::string& sort,
        int limit,
        int offset
    );

    std::optional<Task> GetTaskById(int id);

    bool DeleteTaskById(int id);
    
    std::optional<Task> UpdateTaskById(
        int id,
        const std::string& title,
        const std::string& description,
        const std::string& status,
        const std::string& priority,
        bool completed
    );

private:
    TaskRepository repository_;
};
/*
Шаг 4. TaskService.cpp

Открой:

services/TaskService.cpp
*/