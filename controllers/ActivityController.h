#pragma once

#include <drogon/HttpController.h>

class ActivityController : public drogon::HttpController<ActivityController>
{
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(ActivityController::GetActivity, "/activity", drogon::Get);
    METHOD_LIST_END

    void GetActivity(
        const drogon::HttpRequestPtr &req,
        std::function<void(const drogon::HttpResponsePtr &)> &&callback);
};