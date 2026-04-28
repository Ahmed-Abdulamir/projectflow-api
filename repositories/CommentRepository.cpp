#include "CommentRepository.h"

#include <stdexcept>
#include <vector>

namespace
{
    Comment RowToComment(const drogon::orm::Row &row)
    {
        Comment comment;
        comment.id = row["id"].as<int>();
        comment.taskId = row["task_id"].as<int>();
        comment.content = row["content"].as<std::string>();
        comment.createdAt = row["created_at"].as<std::string>();

        return comment;
    }
}

CommentRepository::CommentRepository(drogon::orm::DbClientPtr client)
    : db_(client) {}

Comment CommentRepository::CreateComment(
    int taskId,
    const std::string &content)
{
    auto result = db_->execSqlSync(
        "INSERT INTO comments (task_id, content) "
        "VALUES ($1, $2) "
        "RETURNING "
        "id, task_id, content, "
        "TO_CHAR(created_at, 'YYYY-MM-DD\"T\"HH24:MI:SS') AS created_at",
        taskId,
        content);

    if (result.empty())
    {
        throw std::runtime_error("Insert comment failed");
    }

    return RowToComment(result[0]);
}

std::vector<Comment> CommentRepository::GetCommentsByTaskId(int taskId)
{
    auto result = db_->execSqlSync(
        "SELECT "
        "id, task_id, content, "
        "TO_CHAR(created_at, 'YYYY-MM-DD\"T\"HH24:MI:SS') AS created_at "
        "FROM comments "
        "WHERE task_id = $1 "
        "ORDER BY id DESC",
        taskId);

    std::vector<Comment> comments;

    for (const auto &row : result)
    {
        comments.push_back(RowToComment(row));
    }

    return comments;
}

bool CommentRepository::DeleteCommentById(int id)
{
    auto result = db_->execSqlSync(
        "DELETE FROM comments WHERE id = $1 RETURNING id",
        id);

    return !result.empty();
}