#include "CommentController.h"

#include "../repositories/CommentRepository.h"
#include "../repositories/TaskRepository.h"
#include "../services/CommentService.h"
#include "../services/TaskService.h"

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

    Json::Value CommentToJson(const Comment &comment)
    {
        Json::Value result;
        result["id"] = comment.id;
        result["task_id"] = comment.taskId;
        result["content"] = comment.content;
        result["created_at"] = comment.createdAt;

        return result;
    }
}

void CommentController::CreateComment(
    const drogon::HttpRequestPtr &req,
    std::function<void(const drogon::HttpResponsePtr &)> &&callback,
    int taskId)
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

        if (!json->isMember("content") || !(*json)["content"].isString())
        {
            callback(MakeErrorResponse(
                "INVALID_COMMENT_CONTENT",
                "Field 'content' is required and must be a string",
                drogon::k400BadRequest));
            return;
        }

        std::string content = (*json)["content"].asString();

        auto dbClient = drogon::app().getDbClient();

        TaskRepository taskRepository(dbClient);
        TaskService taskService(taskRepository);

        auto task = taskService.GetTaskById(taskId);

        if (!task.has_value())
        {
            callback(MakeErrorResponse(
                "TASK_NOT_FOUND",
                "Task not found",
                drogon::k404NotFound));
            return;
        }

        CommentRepository commentRepository(dbClient);
        CommentService commentService(commentRepository);

        Comment comment = commentService.CreateComment(taskId, content);

        Json::Value result = CommentToJson(comment);

        auto response = drogon::HttpResponse::newHttpJsonResponse(result);
        response->setStatusCode(drogon::k201Created);

        callback(response);
    }
    catch (const std::exception &e)
    {
        std::string message = e.what();

        if (message == "Task id must be positive")
        {
            callback(MakeErrorResponse(
                "INVALID_TASK_ID",
                "Task id must be positive",
                drogon::k400BadRequest));
            return;
        }

        if (message == "Comment content must not be empty")
        {
            callback(MakeErrorResponse(
                "INVALID_COMMENT_CONTENT",
                "Comment content must not be empty",
                drogon::k400BadRequest));
            return;
        }

        if (message == "Comment content is too long")
        {
            callback(MakeErrorResponse(
                "INVALID_COMMENT_CONTENT",
                "Comment content must be no longer than 2000 characters",
                drogon::k400BadRequest));
            return;
        }

        callback(MakeErrorResponse(
            "INTERNAL_SERVER_ERROR",
            e.what(),
            drogon::k500InternalServerError));
    }
}

void CommentController::GetTaskComments(
    const drogon::HttpRequestPtr &req,
    std::function<void(const drogon::HttpResponsePtr &)> &&callback,
    int taskId)
{
    try
    {
        auto dbClient = drogon::app().getDbClient();

        TaskRepository taskRepository(dbClient);
        TaskService taskService(taskRepository);

        auto task = taskService.GetTaskById(taskId);

        if (!task.has_value())
        {
            callback(MakeErrorResponse(
                "TASK_NOT_FOUND",
                "Task not found",
                drogon::k404NotFound));
            return;
        }

        CommentRepository commentRepository(dbClient);
        CommentService commentService(commentRepository);

        auto comments = commentService.GetCommentsByTaskId(taskId);

        Json::Value result(Json::arrayValue);

        for (const auto &comment : comments)
        {
            result.append(CommentToJson(comment));
        }

        auto response = drogon::HttpResponse::newHttpJsonResponse(result);
        response->setStatusCode(drogon::k200OK);

        callback(response);
    }
    catch (const std::exception &e)
    {
        std::string message = e.what();

        if (message == "Task id must be positive")
        {
            callback(MakeErrorResponse(
                "INVALID_TASK_ID",
                "Task id must be positive",
                drogon::k400BadRequest));
            return;
        }

        callback(MakeErrorResponse(
            "INTERNAL_SERVER_ERROR",
            e.what(),
            drogon::k500InternalServerError));
    }
}

void CommentController::DeleteComment(
    const drogon::HttpRequestPtr &req,
    std::function<void(const drogon::HttpResponsePtr &)> &&callback,
    int commentId)
{
    try
    {
        auto dbClient = drogon::app().getDbClient();

        CommentRepository repository(dbClient);
        CommentService service(repository);

        bool deleted = service.DeleteCommentById(commentId);

        if (!deleted)
        {
            callback(MakeErrorResponse(
                "COMMENT_NOT_FOUND",
                "Comment not found",
                drogon::k404NotFound));
            return;
        }

        Json::Value result;
        result["message"] = "Comment deleted";

        auto response = drogon::HttpResponse::newHttpJsonResponse(result);
        response->setStatusCode(drogon::k200OK);

        callback(response);
    }
    catch (const std::exception &e)
    {
        std::string message = e.what();

        if (message == "Comment id must be positive")
        {
            callback(MakeErrorResponse(
                "INVALID_COMMENT_ID",
                "Comment id must be positive",
                drogon::k400BadRequest));
            return;
        }

        callback(MakeErrorResponse(
            "INTERNAL_SERVER_ERROR",
            e.what(),
            drogon::k500InternalServerError));
    }
}