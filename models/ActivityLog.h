#pragma once

#include <string>

struct ActivityLog
{
    int id;
    std::string entityType;
    int entityId;
    std::string action;
    std::string description;
    std::string createdAt;
};