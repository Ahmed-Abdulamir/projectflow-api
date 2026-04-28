#include "CommentService.h"

#include <stdexcept>

CommentService::CommentService(CommentRepository repository)
    : repository_(repository) {}

Comment CommentService::CreateComment(
    int taskId,
    const std::string &content)
{
    if (taskId <= 0)
    {
        throw std::runtime_error("Task id must be positive");
    }

    if (content.empty())
    {
        throw std::runtime_error("Comment content must not be empty");
    }

    if (content.size() > 2000)
    {
        throw std::runtime_error("Comment content is too long");
    }

    return repository_.CreateComment(taskId, content);
}

std::vector<Comment> CommentService::GetCommentsByTaskId(int taskId)
{
    if (taskId <= 0)
    {
        throw std::runtime_error("Task id must be positive");
    }

    return repository_.GetCommentsByTaskId(taskId);
}

bool CommentService::DeleteCommentById(int id)
{
    if (id <= 0)
    {
        throw std::runtime_error("Comment id must be positive");
    }

    return repository_.DeleteCommentById(id);
}