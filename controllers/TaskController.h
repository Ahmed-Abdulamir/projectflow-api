#pragma once

#include <drogon/HttpController.h>

class TaskController : public drogon::HttpController<TaskController> {
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(TaskController::Health, "/health", drogon::Get);
    ADD_METHOD_TO(TaskController::DatabaseHealth, "/health/db", drogon::Get);
    ADD_METHOD_TO(TaskController::CreateTask, "/tasks", drogon::Post);
    ADD_METHOD_TO(TaskController::GetTasks, "/tasks", drogon::Get);
    ADD_METHOD_TO(TaskController::GetTaskById, "/tasks/{1}", drogon::Get);
    ADD_METHOD_TO(TaskController::DeleteTaskById, "/tasks/{1}", drogon::Delete);
    ADD_METHOD_TO(TaskController::UpdateTaskById, "/tasks/{1}", drogon::Patch);
    METHOD_LIST_END

    void Health(
    const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback
    );
    
    void CreateTask(
        const drogon::HttpRequestPtr& req,
        std::function<void(const drogon::HttpResponsePtr&)>&& callback
    );

    void GetTasks(
        const drogon::HttpRequestPtr& req,
        std::function<void(const drogon::HttpResponsePtr&)>&& callback
    );

    void GetTaskById(
        const drogon::HttpRequestPtr& req,
        std::function<void(const drogon::HttpResponsePtr&)>&& callback,
        int id
    );

    void DeleteTaskById(
        const drogon::HttpRequestPtr& req,
        std::function<void(const drogon::HttpResponsePtr&)>&& callback,
        int id
    );

    void UpdateTaskById(
    const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback,
    int id
    );

    void DatabaseHealth(
    const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback
    );
};