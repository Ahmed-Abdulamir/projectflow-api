#include "ActivityRepository.h"

#include <vector>

namespace
{
    ActivityLog RowToActivityLog(const drogon::orm::Row &row)
    {
        ActivityLog log;
        log.id = row["id"].as<int>();
        log.entityType = row["entity_type"].as<std::string>();
        log.entityId = row["entity_id"].as<int>();
        log.action = row["action"].as<std::string>();
        log.description = row["description"].as<std::string>();
        log.createdAt = row["created_at"].as<std::string>();

        return log;
    }
}

ActivityRepository::ActivityRepository(drogon::orm::DbClientPtr client)
    : db_(client) {}

void ActivityRepository::CreateActivity(
    const std::string &entityType,
    int entityId,
    const std::string &action,
    const std::string &description)
{
    db_->execSqlSync(
        "INSERT INTO activity_logs (entity_type, entity_id, action, description) "
        "VALUES ($1, $2, $3, $4)",
        entityType,
        entityId,
        action,
        description);
}

std::vector<ActivityLog> ActivityRepository::GetRecentActivity(
    int limit,
    int offset)
{
    auto result = db_->execSqlSync(
        "SELECT "
        "id, entity_type, entity_id, action, description, "
        "TO_CHAR(created_at, 'YYYY-MM-DD\"T\"HH24:MI:SS') AS created_at "
        "FROM activity_logs "
        "ORDER BY id DESC "
        "LIMIT $1::int OFFSET $2::int",
        limit,
        offset);

    std::vector<ActivityLog> logs;

    for (const auto &row : result)
    {
        logs.push_back(RowToActivityLog(row));
    }

    return logs;
}