#pragma once

#include "../models/Project.h"
#include "../repositories/ProjectRepository.h"

#include <string>
#include <vector>
#include <optional>

class ProjectService
{
public:
    explicit ProjectService(ProjectRepository repository);

    Project CreateProject(
        const std::string &name,
        const std::string &description);

    std::vector<Project> GetAllProjects();

    std::optional<Project> GetProjectById(int id);

    std::optional<Project> UpdateProjectById(
        int id,
        const std::string &name,
        const std::string &description);

    bool DeleteProjectById(int id);

private:
    ProjectRepository repository_;
};