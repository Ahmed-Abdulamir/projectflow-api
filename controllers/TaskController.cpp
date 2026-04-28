#include "TaskController.h"

#include "../repositories/TaskRepository.h"
#include "../services/TaskService.h"

#include <drogon/drogon.h>
#include <exception>
#include <string>

#include "TaskController.h"

#include "../repositories/TaskRepository.h"
#include "../services/TaskService.h"

#include <drogon/drogon.h>
#include <exception>
#include <string>

namespace {
drogon::HttpResponsePtr MakeErrorResponse(
    const std::string& code,
    const std::string& message,
    drogon::HttpStatusCode statusCode
) {
    Json::Value body;
    body["error"]["code"] = code;
    body["error"]["message"] = message;

    auto response = drogon::HttpResponse::newHttpJsonResponse(body);
    response->setStatusCode(statusCode);

    return response;
}

Json::Value TaskToJson(const Task& task) {
    Json::Value result;
    result["id"] = task.id;
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

void TaskController::Health(
    const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback
) {
    Json::Value result;
    result["status"] = "ok";
    result["service"] = "ProjectFlow API";

    auto response = drogon::HttpResponse::newHttpJsonResponse(result);
    response->setStatusCode(drogon::k200OK);

    callback(response);
}

void TaskController::DatabaseHealth(
    const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback
) {
    try {
        auto dbClient = drogon::app().getDbClient();

        dbClient->execSqlSync("SELECT 1");

        Json::Value result;
        result["status"] = "ok";
        result["database"] = "connected";

        auto response = drogon::HttpResponse::newHttpJsonResponse(result);
        response->setStatusCode(drogon::k200OK);

        callback(response);
    }
    catch (const std::exception& e) {
        callback(MakeErrorResponse(
            "DATABASE_CONNECTION_ERROR",
            e.what(),
            drogon::k500InternalServerError
        ));
    }
}

void TaskController::CreateTask(
    const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback
) {
    try {
        auto json = req->getJsonObject();

        if (!json) {
            callback(MakeErrorResponse(
                "INVALID_JSON",
                "Request body must be valid JSON",
                drogon::k400BadRequest
            ));
            return;
        }

        if (!json->isMember("title") || !(*json)["title"].isString()) {
            callback(MakeErrorResponse(
                "INVALID_TITLE",
                "Field 'title' is required and must be a string",
                drogon::k400BadRequest
            ));
            return;
        }

        std::string title = (*json)["title"].asString();

        std::string description = "";
        std::string status = "todo";
        std::string priority = "medium";

        if (json->isMember("description")) {
            if (!(*json)["description"].isString()) {
                callback(MakeErrorResponse(
                    "INVALID_DESCRIPTION",
                    "Field 'description' must be a string",
                    drogon::k400BadRequest
                ));
                return;
            }

            description = (*json)["description"].asString();
        }

        if (json->isMember("status")) {
            if (!(*json)["status"].isString()) {
                callback(MakeErrorResponse(
                    "INVALID_STATUS",
                    "Field 'status' must be a string",
                    drogon::k400BadRequest
                ));
                return;
            }

            status = (*json)["status"].asString();
        }

        if (json->isMember("priority")) {
            if (!(*json)["priority"].isString()) {
                callback(MakeErrorResponse(
                    "INVALID_PRIORITY",
                    "Field 'priority' must be a string",
                    drogon::k400BadRequest
                ));
                return;
            }

            priority = (*json)["priority"].asString();
        }

        auto dbClient = drogon::app().getDbClient();

        TaskRepository repository(dbClient);
        TaskService service(repository);

        Task task = service.CreateTask(title, description, status, priority);

        Json::Value result = TaskToJson(task);

        auto response = drogon::HttpResponse::newHttpJsonResponse(result);
        response->setStatusCode(drogon::k201Created);

        callback(response);
    }
    catch (const std::exception& e) {
        std::string message = e.what();

        if (message == "Invalid task status") {
            callback(MakeErrorResponse(
                "INVALID_STATUS",
                "Status must be one of: todo, in_progress, done, cancelled",
                drogon::k400BadRequest
            ));
            return;
        }

        if (message == "Invalid task priority") {
            callback(MakeErrorResponse(
                "INVALID_PRIORITY",
                "Priority must be one of: low, medium, high",
                drogon::k400BadRequest
            ));
            return;
        }

        if (message == "Title must not be empty") {
            callback(MakeErrorResponse(
                "INVALID_TITLE",
                "Title must not be empty",
                drogon::k400BadRequest
            ));
            return;
        }

        callback(MakeErrorResponse(
            "INTERNAL_SERVER_ERROR",
            e.what(),
            drogon::k500InternalServerError
        ));
    }
}

void TaskController::GetTasks(
    const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback
) {
    try {
        std::string status = req->getParameter("status");
        std::string priority = req->getParameter("priority");
        std::string search = req->getParameter("search");

        std::string sort = req->getParameter("sort");

        if (sort.empty())
        {
            sort = "id_desc";
        }

        int limit = 20;
        int offset = 0;

        std::string limitParam = req->getParameter("limit");
        std::string offsetParam = req->getParameter("offset");

        if (!limitParam.empty()) {
            try {
                limit = std::stoi(limitParam);
            }
            catch (...) {
                callback(MakeErrorResponse(
                    "INVALID_LIMIT",
                    "Limit must be a number between 1 and 100",
                    drogon::k400BadRequest
                ));
                return;
            }
        }

        if (!offsetParam.empty()) {
            try {
                offset = std::stoi(offsetParam);
            }
            catch (...) {
                callback(MakeErrorResponse(
                    "INVALID_OFFSET",
                    "Offset must be a non-negative number",
                    drogon::k400BadRequest
                ));
                return;
            }
        }

        auto dbClient = drogon::app().getDbClient();

        TaskRepository repository(dbClient);
        TaskService service(repository);

        auto tasks = service.GetAllTasks(status, priority, search, sort, limit, offset);

        Json::Value result(Json::arrayValue);

        for (const auto& task : tasks) {
            result.append(TaskToJson(task));
        }

        auto response = drogon::HttpResponse::newHttpJsonResponse(result);
        response->setStatusCode(drogon::k200OK);

        callback(response);
    }
    catch (const std::exception& e) {
        std::string message = e.what();

        if (message == "Invalid task status") {
            callback(MakeErrorResponse(
                "INVALID_STATUS",
                "Status must be one of: todo, in_progress, done, cancelled",
                drogon::k400BadRequest
            ));
            return;
        }

        if (message == "Invalid task priority") {
            callback(MakeErrorResponse(
                "INVALID_PRIORITY",
                "Priority must be one of: low, medium, high",
                drogon::k400BadRequest
            ));
            return;
        }

        if (message == "Invalid limit") {
            callback(MakeErrorResponse(
                "INVALID_LIMIT",
                "Limit must be between 1 and 100",
                drogon::k400BadRequest
            ));
            return;
        }

        if (message == "Invalid offset") {
            callback(MakeErrorResponse(
                "INVALID_OFFSET",
                "Offset must be a non-negative number",
                drogon::k400BadRequest
            ));
            return;
        }

        if (message == "Invalid search") {
            callback(MakeErrorResponse(
                "INVALID_SEARCH",
                "Search query must be no longer than 100 characters",
                drogon::k400BadRequest
            ));
            return;
        }

        if (message == "Invalid sort")
        {
            callback(MakeErrorResponse(
                "INVALID_SORT",
                "Sort must be one of: id_desc, id_asc, created_at_desc, created_at_asc, priority_desc, priority_asc, status_asc",
                drogon::k400BadRequest));
            return;
        }

        callback(MakeErrorResponse(
            "INTERNAL_SERVER_ERROR",
            e.what(),
            drogon::k500InternalServerError
        ));
    }
}

void TaskController::GetTaskById(
    const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback,
    int id
) {
    try {
        auto dbClient = drogon::app().getDbClient();

        TaskRepository repository(dbClient);
        TaskService service(repository);

        auto task = service.GetTaskById(id);

        if (!task.has_value()) {
            callback(MakeErrorResponse(
                "TASK_NOT_FOUND",
                "Task not found",
                drogon::k404NotFound
            ));
            return;
        }

        Json::Value result = TaskToJson(*task);

        auto response = drogon::HttpResponse::newHttpJsonResponse(result);
        response->setStatusCode(drogon::k200OK);

        callback(response);
    }
    catch (const std::exception& e) {
        callback(MakeErrorResponse(
            "INTERNAL_SERVER_ERROR",
            e.what(),
            drogon::k500InternalServerError
        ));
    }
}

void TaskController::DeleteTaskById(
    const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback,
    int id
) {
    try {
        auto dbClient = drogon::app().getDbClient();

        TaskRepository repository(dbClient);
        TaskService service(repository);

        bool deleted = service.DeleteTaskById(id);

        if (!deleted) {
            callback(MakeErrorResponse(
                "TASK_NOT_FOUND",
                "Task not found",
                drogon::k404NotFound
            ));
            return;
        }

        Json::Value result;
        result["message"] = "Task deleted";

        auto response = drogon::HttpResponse::newHttpJsonResponse(result);
        response->setStatusCode(drogon::k200OK);

        callback(response);
    }
    catch (const std::exception& e) {
        callback(MakeErrorResponse(
            "INTERNAL_SERVER_ERROR",
            e.what(),
            drogon::k500InternalServerError
        ));
    }
}

void TaskController::UpdateTaskById(
    const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback,
    int id
) {
    try {
        auto json = req->getJsonObject();

        if (!json) {
            callback(MakeErrorResponse(
                "INVALID_JSON",
                "Request body must be valid JSON",
                drogon::k400BadRequest
            ));
            return;
        }

        if (!json->isMember("title") || !(*json)["title"].isString()) {
            callback(MakeErrorResponse(
                "INVALID_TITLE",
                "Field 'title' is required and must be a string",
                drogon::k400BadRequest
            ));
            return;
        }

        if (!json->isMember("description") || !(*json)["description"].isString()) {
            callback(MakeErrorResponse(
                "INVALID_DESCRIPTION",
                "Field 'description' is required and must be a string",
                drogon::k400BadRequest
            ));
            return;
        }

        if (!json->isMember("status") || !(*json)["status"].isString()) {
            callback(MakeErrorResponse(
                "INVALID_STATUS",
                "Field 'status' is required and must be a string",
                drogon::k400BadRequest
            ));
            return;
        }

        if (!json->isMember("priority") || !(*json)["priority"].isString()) {
            callback(MakeErrorResponse(
                "INVALID_PRIORITY",
                "Field 'priority' is required and must be a string",
                drogon::k400BadRequest
            ));
            return;
        }

        if (!json->isMember("completed") || !(*json)["completed"].isBool()) {
            callback(MakeErrorResponse(
                "INVALID_COMPLETED",
                "Field 'completed' is required and must be a boolean",
                drogon::k400BadRequest
            ));
            return;
        }

        std::string title = (*json)["title"].asString();
        std::string description = (*json)["description"].asString();
        std::string status = (*json)["status"].asString();
        std::string priority = (*json)["priority"].asString();
        bool completed = (*json)["completed"].asBool();

        auto dbClient = drogon::app().getDbClient();

        TaskRepository repository(dbClient);
        TaskService service(repository);

        auto task = service.UpdateTaskById(
            id,
            title,
            description,
            status,
            priority,
            completed
        );

        if (!task.has_value()) {
            callback(MakeErrorResponse(
                "TASK_NOT_FOUND",
                "Task not found",
                drogon::k404NotFound
            ));
            return;
        }

        Json::Value result = TaskToJson(*task);

        auto response = drogon::HttpResponse::newHttpJsonResponse(result);
        response->setStatusCode(drogon::k200OK);

        callback(response);
    }
    catch (const std::exception& e) {
        std::string message = e.what();

        if (message == "Invalid task status") {
            callback(MakeErrorResponse(
                "INVALID_STATUS",
                "Status must be one of: todo, in_progress, done, cancelled",
                drogon::k400BadRequest
            ));
            return;
        }

        if (message == "Invalid task priority") {
            callback(MakeErrorResponse(
                "INVALID_PRIORITY",
                "Priority must be one of: low, medium, high",
                drogon::k400BadRequest
            ));
            return;
        }

        if (message == "Title must not be empty") {
            callback(MakeErrorResponse(
                "INVALID_TITLE",
                "Title must not be empty",
                drogon::k400BadRequest
            ));
            return;
        }

        callback(MakeErrorResponse(
            "INTERNAL_SERVER_ERROR",
            e.what(),
            drogon::k500InternalServerError
        ));
    }
}