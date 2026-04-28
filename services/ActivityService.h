#pragma once

#include "../models/ActivityLog.h"
#include "../repositories/ActivityRepository.h"

#include <string>
#include <vector>

class ActivityService
{
public:
    explicit ActivityService(ActivityRepository repository);

    void CreateActivity(
        const std::string &entityType,
        int entityId,
        const std::string &action,
        const std::string &description);

    std::vector<ActivityLog> GetRecentActivity(
        int limit,
        int offset);

private:
    ActivityRepository repository_;
};