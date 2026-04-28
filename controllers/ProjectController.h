#pragma once

#include <drogon/HttpController.h>

class ProjectController : public drogon::HttpController<ProjectController>
{
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(ProjectController::CreateProject, "/projects", drogon::Post);
    ADD_METHOD_TO(ProjectController::GetProjects, "/projects", drogon::Get);
    ADD_METHOD_TO(ProjectController::GetProjectById, "/projects/{1}", drogon::Get);
    ADD_METHOD_TO(ProjectController::UpdateProjectById, "/projects/{1}", drogon::Patch);
    ADD_METHOD_TO(ProjectController::DeleteProjectById, "/projects/{1}", drogon::Delete);
    METHOD_LIST_END

    void CreateProject(
        const drogon::HttpRequestPtr &req,
        std::function<void(const drogon::HttpResponsePtr &)> &&callback);

    void GetProjects(
        const drogon::HttpRequestPtr &req,
        std::function<void(const drogon::HttpResponsePtr &)> &&callback);

    void GetProjectById(
        const drogon::HttpRequestPtr &req,
        std::function<void(const drogon::HttpResponsePtr &)> &&callback,
        int id);

    void UpdateProjectById(
        const drogon::HttpRequestPtr &req,
        std::function<void(const drogon::HttpResponsePtr &)> &&callback,
        int id);

    void DeleteProjectById(
        const drogon::HttpRequestPtr &req,
        std::function<void(const drogon::HttpResponsePtr &)> &&callback,
        int id);
};
