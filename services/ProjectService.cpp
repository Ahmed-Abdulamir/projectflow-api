#include "ProjectService.h"

#include <stdexcept> 
#include <optional> 

ProjectService::ProjectService(ProjectRepository repository)
    : repository_(repository) {}

Project ProjectService::CreateProject(
    const std::string &name,
    const std::string &description)
{
    if (name.empty())
    {
        throw std::runtime_error("Project name must not be empty");
    }

    if (name.size() > 100)
    {
        throw std::runtime_error("Project name is too long");
    }

    if (description.size() > 1000)
    {
        throw std::runtime_error("Project description is too long");
    }

    return repository_.CreateProject(name, description);
}

std::vector<Project> ProjectService::GetAllProjects()
{
    return repository_.GetAllProjects();
}

std::optional<Project> ProjectService::GetProjectById(int id)
{
    if (id <= 0)
    {
        throw std::runtime_error("Project id must be positive");
    }

    return repository_.GetProjectById(id);
}

std::optional<Project> ProjectService::UpdateProjectById(
    int id,
    const std::string &name,
    const std::string &description)
{
    if (id <= 0)
    {
        throw std::runtime_error("Project id must be positive");
    }

    if (name.empty())
    {
        throw std::runtime_error("Project name must not be empty");
    }

    if (name.size() > 100)
    {
        throw std::runtime_error("Project name is too long");
    }

    if (description.size() > 1000)
    {
        throw std::runtime_error("Project description is too long");
    }

    return repository_.UpdateProjectById(id, name, description);
}

bool ProjectService::DeleteProjectById(int id)
{
    if (id <= 0)
    {
        throw std::runtime_error("Project id must be positive");
    }

    return repository_.DeleteProjectById(id);
}