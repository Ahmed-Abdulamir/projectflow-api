#pragma once

#include "../models/Comment.h"
#include "../repositories/CommentRepository.h"

#include <string>
#include <vector>

class CommentService
{
public:
    explicit CommentService(CommentRepository repository);

    Comment CreateComment(
        int taskId,
        const std::string &content);

    std::vector<Comment> GetCommentsByTaskId(int taskId);

    bool DeleteCommentById(int id);

private:
    CommentRepository repository_;
};