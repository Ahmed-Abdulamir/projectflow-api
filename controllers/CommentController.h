#pragma once

#include <drogon/HttpController.h>

class CommentController : public drogon::HttpController<CommentController>
{
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(CommentController::CreateComment, "/tasks/{1}/comments", drogon::Post);
    ADD_METHOD_TO(CommentController::GetTaskComments, "/tasks/{1}/comments", drogon::Get);
    ADD_METHOD_TO(CommentController::DeleteComment, "/comments/{1}", drogon::Delete);
    METHOD_LIST_END

    void CreateComment(
        const drogon::HttpRequestPtr &req,
        std::function<void(const drogon::HttpResponsePtr &)> &&callback,
        int taskId);

    void GetTaskComments(
        const drogon::HttpRequestPtr &req,
        std::function<void(const drogon::HttpResponsePtr &)> &&callback,
        int taskId);

    void DeleteComment(
        const drogon::HttpRequestPtr &req,
        std::function<void(const drogon::HttpResponsePtr &)> &&callback,
        int commentId);
};