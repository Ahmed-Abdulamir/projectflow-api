#pragma once

#include "../models/ActivityLog.h"

#include <drogon/orm/DbClient.h>
#include <string>
#include <vector>

class ActivityRepository
{
public:
    explicit ActivityRepository(drogon::orm::DbClientPtr client);

    void CreateActivity(
        const std::string &entityType,
        int entityId,
        const std::string &action,
        const std::string &description);

    std::vector<ActivityLog> GetRecentActivity(
        int limit,
        int offset);

private:
    drogon::orm::DbClientPtr db_;
};