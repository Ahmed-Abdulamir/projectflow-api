#include "TaskService.h"

#include <optional>
#include <stdexcept>

TaskService::TaskService(TaskRepository repository)
    : repository_(repository) {}

Task TaskService::CreateTask(
    int projectId,
    const std::string &title,
    const std::string &description,
    const std::string &status,
    const std::string &priority)
{
    if (projectId < 0)
    {
        throw std::runtime_error("Project id must be positive");
    }

    if (title.empty())
    {
        throw std::runtime_error("Title must not be empty");
    }

    if (status != "todo" && status != "in_progress" && status != "done" && status != "cancelled")
    {
        throw std::runtime_error("Invalid task status");
    }

    if (priority != "low" && priority != "medium" && priority != "high")
    {
        throw std::runtime_error("Invalid task priority");
    }

    return repository_.CreateTask(projectId, title, description, status, priority);
}

std::vector<Task> TaskService::GetAllTasks(
    const std::string& status,
    const std::string& priority,
    const std::string& search,
    const std::string& sort,
    int limit,
    int offset
) {
    if (!status.empty() &&
        status != "todo" &&
        status != "in_progress" &&
        status != "done" &&
        status != "cancelled") {
        throw std::runtime_error("Invalid task status");
    }

    if (!priority.empty() &&
        priority != "low" &&
        priority != "medium" &&
        priority != "high") {
        throw std::runtime_error("Invalid task priority");
    }

    if (search.size() > 100) {
        throw std::runtime_error("Invalid search");
    }

    if (sort != "id_desc" &&
        sort != "id_asc" &&
        sort != "created_at_desc" &&
        sort != "created_at_asc" &&
        sort != "priority_desc" &&
        sort != "priority_asc" &&
        sort != "status_asc") {
        throw std::runtime_error("Invalid sort");
    }

    if (limit <= 0 || limit > 100) {
        throw std::runtime_error("Invalid limit");
    }

    if (offset < 0) {
        throw std::runtime_error("Invalid offset");
    }

    return repository_.GetAllTasks(status, priority, search, sort, limit, offset);
}

std::optional<Task> TaskService::GetTaskById(int id) {
    if (id <= 0) {
        throw std::runtime_error("Task id must be positive");
    }

    return repository_.GetTaskById(id);
}

bool TaskService::DeleteTaskById(int id) {
    if (id <= 0) {
        throw std::runtime_error("Task id must be positive");
    }

    return repository_.DeleteTaskById(id);
}

std::optional<Task> TaskService::UpdateTaskById(
    int id,
    const std::string& title,
    const std::string& description,
    const std::string& status,
    const std::string& priority,
    bool completed
) {
    if (id <= 0) {
        throw std::runtime_error("Task id must be positive");
    }

    if (title.empty()) {
        throw std::runtime_error("Title must not be empty");
    }

    if (status != "todo" && status != "in_progress" && status != "done" && status != "cancelled") {
        throw std::runtime_error("Invalid task status");
    }

    if (priority != "low" && priority != "medium" && priority != "high") {
        throw std::runtime_error("Invalid task priority");
    }

    return repository_.UpdateTaskById(
        id,
        title,
        description,
        status,
        priority,
        completed
    );
}

std::vector<Task> TaskService::GetTasksByProjectId(int projectId)
{
    if (projectId <= 0)
    {
        throw std::runtime_error("Project id must be positive");
    }

    return repository_.GetTasksByProjectId(projectId);
}


