#include "ActivityService.h"

#include <stdexcept>

ActivityService::ActivityService(ActivityRepository repository)
    : repository_(repository) {}

void ActivityService::CreateActivity(
    const std::string &entityType,
    int entityId,
    const std::string &action,
    const std::string &description)
{
    if (entityType.empty())
    {
        throw std::runtime_error("Entity type must not be empty");
    }

    if (entityId <= 0)
    {
        throw std::runtime_error("Entity id must be positive");
    }

    if (action.empty())
    {
        throw std::runtime_error("Action must not be empty");
    }

    repository_.CreateActivity(entityType, entityId, action, description);
}

std::vector<ActivityLog> ActivityService::GetRecentActivity(
    int limit,
    int offset)
{
    if (limit <= 0 || limit > 100)
    {
        throw std::runtime_error("Invalid limit");
    }

    if (offset < 0)
    {
        throw std::runtime_error("Invalid offset");
    }

    return repository_.GetRecentActivity(limit, offset);
}