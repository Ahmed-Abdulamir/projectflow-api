#pragma once

#include <string>

struct Task {
    int id;
    std::string title;
    std::string description;
    std::string status;
    std::string priority;
    bool completed;
    std::string createdAt;
    std::string updatedAt;
};