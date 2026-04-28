#include "TaskRepository.h"

#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

namespace
{
    Task RowToTask(const drogon::orm::Row &row)
    {
        Task task;

        task.id = row["id"].as<int>();

        if (row["project_id"].isNull())
        {
            task.projectId = 0;
        }
        else
        {
            task.projectId = row["project_id"].as<int>();
        }

        task.title = row["title"].as<std::string>();
        task.description = row["description"].as<std::string>();
        task.status = row["status"].as<std::string>();
        task.priority = row["priority"].as<std::string>();
        task.completed = row["completed"].as<bool>();
        task.createdAt = row["created_at"].as<std::string>();
        task.updatedAt = row["updated_at"].as<std::string>();

        return task;
    }
}

TaskRepository::TaskRepository(drogon::orm::DbClientPtr client)
    : db_(client) {}

Task TaskRepository::CreateTask(
    int projectId,
    const std::string &title,
    const std::string &description,
    const std::string &status,
    const std::string &priority)
{
    auto result = db_->execSqlSync(
        "INSERT INTO tasks (project_id, title, description, status, priority) "
        "VALUES (NULLIF($1::int, 0), $2, $3, $4, $5) "
        "RETURNING "
        "id, project_id, title, description, status, priority, completed, "
        "TO_CHAR(created_at, 'YYYY-MM-DD\"T\"HH24:MI:SS') AS created_at, "
        "TO_CHAR(updated_at, 'YYYY-MM-DD\"T\"HH24:MI:SS') AS updated_at",
        projectId,
        title,
        description,
        status,
        priority);

    if (result.empty())
    {
        throw std::runtime_error("Insert failed");
    }

    return RowToTask(result[0]);
}

std::vector<Task> TaskRepository::GetAllTasks(
    const std::string &status,
    const std::string &priority,
    const std::string &search,
    const std::string &sort,
    int limit,
    int offset)
{
    std::string orderBy = "id DESC";

    if (sort == "id_asc")
    {
        orderBy = "id ASC";
    }
    else if (sort == "id_desc")
    {
        orderBy = "id DESC";
    }
    else if (sort == "created_at_asc")
    {
        orderBy = "created_at ASC";
    }
    else if (sort == "created_at_desc")
    {
        orderBy = "created_at DESC";
    }
    else if (sort == "priority_asc")
    {
        orderBy =
            "CASE priority "
            "WHEN 'low' THEN 1 "
            "WHEN 'medium' THEN 2 "
            "WHEN 'high' THEN 3 "
            "ELSE 4 END ASC";
    }
    else if (sort == "priority_desc")
    {
        orderBy =
            "CASE priority "
            "WHEN 'high' THEN 1 "
            "WHEN 'medium' THEN 2 "
            "WHEN 'low' THEN 3 "
            "ELSE 4 END ASC";
    }
    else if (sort == "status_asc")
    {
        orderBy =
            "CASE status "
            "WHEN 'todo' THEN 1 "
            "WHEN 'in_progress' THEN 2 "
            "WHEN 'done' THEN 3 "
            "WHEN 'cancelled' THEN 4 "
            "ELSE 5 END ASC";
    }

    std::string sql =
        "SELECT "
        "id, project_id, title, description, status, priority, completed, "
        "TO_CHAR(created_at, 'YYYY-MM-DD\"T\"HH24:MI:SS') AS created_at, "
        "TO_CHAR(updated_at, 'YYYY-MM-DD\"T\"HH24:MI:SS') AS updated_at "
        "FROM tasks "
        "WHERE ($1 = '' OR status = $1) "
        "AND ($2 = '' OR priority = $2) "
        "AND ($3 = '' OR title ILIKE '%' || $3 || '%' OR description ILIKE '%' || $3 || '%') "
        "ORDER BY " +
        orderBy + " "
                  "LIMIT $4::int OFFSET $5::int";

    auto result = db_->execSqlSync(sql, status, priority, search, limit, offset);

    std::vector<Task> tasks;

    for (const auto &row : result)
    {
        tasks.push_back(RowToTask(row));
    }

    return tasks;
}

std::optional<Task> TaskRepository::GetTaskById(int id)
{
    auto result = db_->execSqlSync(
        "SELECT "
        "id, project_id, title, description, status, priority, completed, "
        "TO_CHAR(created_at, 'YYYY-MM-DD\"T\"HH24:MI:SS') AS created_at, "
        "TO_CHAR(updated_at, 'YYYY-MM-DD\"T\"HH24:MI:SS') AS updated_at "
        "FROM tasks "
        "WHERE id = $1",
        id);

    if (result.empty())
    {
        return std::nullopt;
    }

    return RowToTask(result[0]);
}

bool TaskRepository::DeleteTaskById(int id)
{
    auto result = db_->execSqlSync(
        "DELETE FROM tasks WHERE id = $1 RETURNING id",
        id);

    return !result.empty();
}

std::optional<Task> TaskRepository::UpdateTaskById(
    int id,
    const std::string &title,
    const std::string &description,
    const std::string &status,
    const std::string &priority,
    bool completed)
    
{
    auto result = db_->execSqlSync(
        "UPDATE tasks "
        "SET title = $1, "
        "description = $2, "
        "status = $3, "
        "priority = $4, "
        "completed = $5, "
        "updated_at = NOW() "
        "WHERE id = $6 "
        "RETURNING "
        "id, project_id, title, description, status, priority, completed, "
        "TO_CHAR(created_at, 'YYYY-MM-DD\"T\"HH24:MI:SS') AS created_at, "
        "TO_CHAR(updated_at, 'YYYY-MM-DD\"T\"HH24:MI:SS') AS updated_at",
        title,
        description,
        status,
        priority,
        completed,
        id);

    if (result.empty())
    {
        return std::nullopt;
    }

    return RowToTask(result[0]);
}

std::vector<Task> TaskRepository::GetTasksByProjectId(int projectId)
{
    auto result = db_->execSqlSync(
        "SELECT "
        "id, project_id, title, description, status, priority, completed, "
        "TO_CHAR(created_at, 'YYYY-MM-DD\"T\"HH24:MI:SS') AS created_at, "
        "TO_CHAR(updated_at, 'YYYY-MM-DD\"T\"HH24:MI:SS') AS updated_at "
        "FROM tasks "
        "WHERE project_id = $1 "
        "ORDER BY id DESC",
        projectId);

    std::vector<Task> tasks;

    for (const auto &row : result)
    {
        tasks.push_back(RowToTask(row));
    }

    return tasks;
}



