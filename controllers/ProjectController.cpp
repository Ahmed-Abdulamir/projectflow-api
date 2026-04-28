#include "ProjectController.h"

#include "../repositories/ProjectRepository.h"
#include "../services/ProjectService.h"
#include "../repositories/TaskRepository.h"
#include "../services/TaskService.h"
#include "../repositories/ActivityRepository.h"
#include "../services/ActivityService.h"

#include <drogon/drogon.h>
#include <exception>
#include <string>

namespace
{
    drogon::HttpResponsePtr MakeErrorResponse(
        const std::string &code,
        const std::string &message,
        drogon::HttpStatusCode statusCode)
    {
        Json::Value body;
        body["error"]["code"] = code;
        body["error"]["message"] = message;

        auto response = drogon::HttpResponse::newHttpJsonResponse(body);
        response->setStatusCode(statusCode);

        return response;
    }

    Json::Value ProjectToJson(const Project &project)
    {
        Json::Value result;
        result["id"] = project.id;
        result["name"] = project.name;
        result["description"] = project.description;
        result["created_at"] = project.createdAt;
        result["updated_at"] = project.updatedAt;

        return result;
    }

    Json::Value TaskToJson(const Task &task)
    {
        Json::Value result;
        result["id"] = task.id;

        if (task.projectId == 0)
        {
            result["project_id"] = Json::nullValue;
        }
        else
        {
            result["project_id"] = task.projectId;
        }

        result["title"] = task.title;
        result["description"] = task.description;
        result["status"] = task.status;
        result["priority"] = task.priority;
        result["completed"] = task.completed;
        result["created_at"] = task.createdAt;
        result["updated_at"] = task.updatedAt;

        return result;
    }
}

void ProjectController::CreateProject(
    const drogon::HttpRequestPtr &req,
    std::function<void(const drogon::HttpResponsePtr &)> &&callback)
{
    try
    {
        auto json = req->getJsonObject();

        if (!json)
        {
            callback(MakeErrorResponse(
                "INVALID_JSON",
                "Request body must be valid JSON",
                drogon::k400BadRequest));
            return;
        }

        if (!json->isMember("name") || !(*json)["name"].isString())
        {
            callback(MakeErrorResponse(
                "INVALID_PROJECT_NAME",
                "Field 'name' is required and must be a string",
                drogon::k400BadRequest));
            return;
        }

        std::string name = (*json)["name"].asString();
        std::string description = "";

        if (json->isMember("description"))
        {
            if (!(*json)["description"].isString())
            {
                callback(MakeErrorResponse(
                    "INVALID_PROJECT_DESCRIPTION",
                    "Field 'description' must be a string",
                    drogon::k400BadRequest));
                return;
            }

            description = (*json)["description"].asString();
        }

        auto dbClient = drogon::app().getDbClient();

        ProjectRepository repository(dbClient);
        ProjectService service(repository);

        Project project = service.CreateProject(name, description);

        ActivityRepository activityRepository(dbClient);
        ActivityService activityService(activityRepository);

        activityService.CreateActivity(
            "PROJECT",
            project.id,
            "PROJECT_CREATED",
            "Project created: " + project.name);

        Json::Value result = ProjectToJson(project);

        auto response = drogon::HttpResponse::newHttpJsonResponse(result);
        response->setStatusCode(drogon::k201Created);

        callback(response);
    }
    catch (const std::exception &e)
    {
        std::string message = e.what();

        if (message == "Project name must not be empty")
        {
            callback(MakeErrorResponse(
                "INVALID_PROJECT_NAME",
                "Project name must not be empty",
                drogon::k400BadRequest));
            return;
        }

        if (message == "Project name is too long")
        {
            callback(MakeErrorResponse(
                "INVALID_PROJECT_NAME",
                "Project name must be no longer than 100 characters",
                drogon::k400BadRequest));
            return;
        }

        if (message == "Project description is too long")
        {
            callback(MakeErrorResponse(
                "INVALID_PROJECT_DESCRIPTION",
                "Project description must be no longer than 1000 characters",
                drogon::k400BadRequest));
            return;
        }

        callback(MakeErrorResponse(
            "INTERNAL_SERVER_ERROR",
            e.what(),
            drogon::k500InternalServerError));
    }
}

void ProjectController::GetProjects(
    const drogon::HttpRequestPtr &req,
    std::function<void(const drogon::HttpResponsePtr &)> &&callback)
{
    try
    {
        auto dbClient = drogon::app().getDbClient();

        ProjectRepository repository(dbClient);
        ProjectService service(repository);

        auto projects = service.GetAllProjects();

        Json::Value result(Json::arrayValue);

        for (const auto &project : projects)
        {
            result.append(ProjectToJson(project));
        }

        auto response = drogon::HttpResponse::newHttpJsonResponse(result);
        response->setStatusCode(drogon::k200OK);

        callback(response);
    }
    catch (const std::exception &e)
    {
        callback(MakeErrorResponse(
            "INTERNAL_SERVER_ERROR",
            e.what(),
            drogon::k500InternalServerError));
    }
}

void ProjectController::GetProjectById(
    const drogon::HttpRequestPtr &req,
    std::function<void(const drogon::HttpResponsePtr &)> &&callback,
    int id)
{
    try
    {
        auto dbClient = drogon::app().getDbClient();

        ProjectRepository repository(dbClient);
        ProjectService service(repository);

        auto project = service.GetProjectById(id);

        if (!project.has_value())
        {
            callback(MakeErrorResponse(
                "PROJECT_NOT_FOUND",
                "Project not found",
                drogon::k404NotFound));
            return;
        }

        Json::Value result = ProjectToJson(*project);

        auto response = drogon::HttpResponse::newHttpJsonResponse(result);
        response->setStatusCode(drogon::k200OK);

        callback(response);
    }
    catch (const std::exception &e)
    {
        std::string message = e.what();

        if (message == "Project id must be positive")
        {
            callback(MakeErrorResponse(
                "INVALID_PROJECT_ID",
                "Project id must be positive",
                drogon::k400BadRequest));
            return;
        }

        callback(MakeErrorResponse(
            "INTERNAL_SERVER_ERROR",
            e.what(),
            drogon::k500InternalServerError));
    }
}

void ProjectController::UpdateProjectById(
    const drogon::HttpRequestPtr &req,
    std::function<void(const drogon::HttpResponsePtr &)> &&callback,
    int id)
{
    try
    {
        auto json = req->getJsonObject();

        if (!json)
        {
            callback(MakeErrorResponse(
                "INVALID_JSON",
                "Request body must be valid JSON",
                drogon::k400BadRequest));
            return;
        }

        if (!json->isMember("name") || !(*json)["name"].isString())
        {
            callback(MakeErrorResponse(
                "INVALID_PROJECT_NAME",
                "Field 'name' is required and must be a string",
                drogon::k400BadRequest));
            return;
        }

        if (!json->isMember("description") || !(*json)["description"].isString())
        {
            callback(MakeErrorResponse(
                "INVALID_PROJECT_DESCRIPTION",
                "Field 'description' is required and must be a string",
                drogon::k400BadRequest));
            return;
        }

        std::string name = (*json)["name"].asString();
        std::string description = (*json)["description"].asString();

        auto dbClient = drogon::app().getDbClient();

        ProjectRepository repository(dbClient);
        ProjectService service(repository);

        auto project = service.UpdateProjectById(id, name, description);

        if (!project.has_value())
        {
            callback(MakeErrorResponse(
                "PROJECT_NOT_FOUND",
                "Project not found",
                drogon::k404NotFound));
            return;
        }

        ActivityRepository activityRepository(dbClient);
        ActivityService activityService(activityRepository);

        activityService.CreateActivity(
            "PROJECT",
            project->id,
            "PROJECT_UPDATED",
            "Project updated: " + project->name);

        Json::Value result = ProjectToJson(*project);

        auto response = drogon::HttpResponse::newHttpJsonResponse(result);
        response->setStatusCode(drogon::k200OK);

        callback(response);
    }
    catch (const std::exception &e)
    {
        std::string message = e.what();

        if (message == "Project id must be positive")
        {
            callback(MakeErrorResponse(
                "INVALID_PROJECT_ID",
                "Project id must be positive",
                drogon::k400BadRequest));
            return;
        }

        if (message == "Project name must not be empty")
        {
            callback(MakeErrorResponse(
                "INVALID_PROJECT_NAME",
                "Project name must not be empty",
                drogon::k400BadRequest));
            return;
        }

        if (message == "Project name is too long")
        {
            callback(MakeErrorResponse(
                "INVALID_PROJECT_NAME",
                "Project name must be no longer than 100 characters",
                drogon::k400BadRequest));
            return;
        }

        if (message == "Project description is too long")
        {
            callback(MakeErrorResponse(
                "INVALID_PROJECT_DESCRIPTION",
                "Project description must be no longer than 1000 characters",
                drogon::k400BadRequest));
            return;
        }

        callback(MakeErrorResponse(
            "INTERNAL_SERVER_ERROR",
            e.what(),
            drogon::k500InternalServerError));
    }
}

void ProjectController::DeleteProjectById(
    const drogon::HttpRequestPtr &req,
    std::function<void(const drogon::HttpResponsePtr &)> &&callback,
    int id)
{
    try
    {
        auto dbClient = drogon::app().getDbClient();

        ProjectRepository repository(dbClient);
        ProjectService service(repository);

        bool deleted = service.DeleteProjectById(id);

        if (!deleted)
        {
            callback(MakeErrorResponse(
                "PROJECT_NOT_FOUND",
                "Project not found",
                drogon::k404NotFound));
            return;
        }

        ActivityRepository activityRepository(dbClient);
        ActivityService activityService(activityRepository);

        activityService.CreateActivity(
            "PROJECT",
            id,
            "PROJECT_DELETED",
            "Project deleted");

        Json::Value result;
        result["message"] = "Project deleted";

        auto response = drogon::HttpResponse::newHttpJsonResponse(result);
        response->setStatusCode(drogon::k200OK);

        callback(response);
    }
    catch (const std::exception &e)
    {
        std::string message = e.what();

        if (message == "Project id must be positive")
        {
            callback(MakeErrorResponse(
                "INVALID_PROJECT_ID",
                "Project id must be positive",
                drogon::k400BadRequest));
            return;
        }

        callback(MakeErrorResponse(
            "INTERNAL_SERVER_ERROR",
            e.what(),
            drogon::k500InternalServerError));
    }
}

void ProjectController::GetProjectTasks(
    const drogon::HttpRequestPtr &req,
    std::function<void(const drogon::HttpResponsePtr &)> &&callback,
    int id)
{
    try
    {
        auto dbClient = drogon::app().getDbClient();

        ProjectRepository projectRepository(dbClient);
        ProjectService projectService(projectRepository);

        auto project = projectService.GetProjectById(id);

        if (!project.has_value())
        {
            callback(MakeErrorResponse(
                "PROJECT_NOT_FOUND",
                "Project not found",
                drogon::k404NotFound));
            return;
        }

        TaskRepository taskRepository(dbClient);
        TaskService taskService(taskRepository);

        auto tasks = taskService.GetTasksByProjectId(id);

        Json::Value result(Json::arrayValue);

        for (const auto &task : tasks)
        {
            result.append(TaskToJson(task));
        }

        auto response = drogon::HttpResponse::newHttpJsonResponse(result);
        response->setStatusCode(drogon::k200OK);

        callback(response);
    }
    catch (const std::exception &e)
    {
        std::string message = e.what();

        if (message == "Project id must be positive")
        {
            callback(MakeErrorResponse(
                "INVALID_PROJECT_ID",
                "Project id must be positive",
                drogon::k400BadRequest));
            return;
        }

        callback(MakeErrorResponse(
            "INTERNAL_SERVER_ERROR",
            e.what(),
            drogon::k500InternalServerError));
    }
}