#pragma once

#include "../models/Project.h"

#include <drogon/orm/DbClient.h>
#include <string>
#include <vector>
#include <optional>

class ProjectRepository
{
public:
    explicit ProjectRepository(drogon::orm::DbClientPtr client);

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
    drogon::orm::DbClientPtr db_;
};