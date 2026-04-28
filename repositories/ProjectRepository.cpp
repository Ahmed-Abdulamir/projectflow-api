#include "ProjectRepository.h"

#include <stdexcept>
#include <vector>
#include <optional>

namespace
{
    Project RowToProject(const drogon::orm::Row &row)
    {
        Project project;
        project.id = row["id"].as<int>();
        project.name = row["name"].as<std::string>();
        project.description = row["description"].as<std::string>();
        project.createdAt = row["created_at"].as<std::string>();
        project.updatedAt = row["updated_at"].as<std::string>();

        return project;
    }
}

ProjectRepository::ProjectRepository(drogon::orm::DbClientPtr client)
    : db_(client) {}

Project ProjectRepository::CreateProject(
    const std::string &name,
    const std::string &description)
{
    auto result = db_->execSqlSync(
        "INSERT INTO projects (name, description) "
        "VALUES ($1, $2) "
        "RETURNING "
        "id, name, description, "
        "TO_CHAR(created_at, 'YYYY-MM-DD\"T\"HH24:MI:SS') AS created_at, "
        "TO_CHAR(updated_at, 'YYYY-MM-DD\"T\"HH24:MI:SS') AS updated_at",
        name,
        description);

    if (result.empty())
    {
        throw std::runtime_error("Insert project failed");
    }

    return RowToProject(result[0]);
}

std::vector<Project> ProjectRepository::GetAllProjects()
{
    auto result = db_->execSqlSync(
        "SELECT "
        "id, name, description, "
        "TO_CHAR(created_at, 'YYYY-MM-DD\"T\"HH24:MI:SS') AS created_at, "
        "TO_CHAR(updated_at, 'YYYY-MM-DD\"T\"HH24:MI:SS') AS updated_at "
        "FROM projects "
        "ORDER BY id DESC");

    std::vector<Project> projects;

    for (const auto &row : result)
    {
        projects.push_back(RowToProject(row));
    }

    return projects;
}

std::optional<Project> ProjectRepository::GetProjectById(int id)
{
    auto result = db_->execSqlSync(
        "SELECT "
        "id, name, description, "
        "TO_CHAR(created_at, 'YYYY-MM-DD\"T\"HH24:MI:SS') AS created_at, "
        "TO_CHAR(updated_at, 'YYYY-MM-DD\"T\"HH24:MI:SS') AS updated_at "
        "FROM projects "
        "WHERE id = $1",
        id);

    if (result.empty())
    {
        return std::nullopt;
    }

    return RowToProject(result[0]);
}

std::optional<Project> ProjectRepository::UpdateProjectById(
    int id,
    const std::string &name,
    const std::string &description)
{
    auto result = db_->execSqlSync(
        "UPDATE projects "
        "SET name = $1, description = $2, updated_at = NOW() "
        "WHERE id = $3 "
        "RETURNING "
        "id, name, description, "
        "TO_CHAR(created_at, 'YYYY-MM-DD\"T\"HH24:MI:SS') AS created_at, "
        "TO_CHAR(updated_at, 'YYYY-MM-DD\"T\"HH24:MI:SS') AS updated_at",
        name,
        description,
        id);

    if (result.empty())
    {
        return std::nullopt;
    }

    return RowToProject(result[0]);
}

bool ProjectRepository::DeleteProjectById(int id)
{
    auto result = db_->execSqlSync(
        "DELETE FROM projects WHERE id = $1 RETURNING id",
        id);

    return !result.empty();
}