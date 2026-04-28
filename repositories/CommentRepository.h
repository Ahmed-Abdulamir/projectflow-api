#pragma once

#include "../models/Comment.h"

#include <drogon/orm/DbClient.h>
#include <string>
#include <vector>

class CommentRepository
{
public:
    explicit CommentRepository(drogon::orm::DbClientPtr client);

    Comment CreateComment(
        int taskId,
        const std::string &content);

    std::vector<Comment> GetCommentsByTaskId(int taskId);

    bool DeleteCommentById(int id);

private:
    drogon::orm::DbClientPtr db_;
};