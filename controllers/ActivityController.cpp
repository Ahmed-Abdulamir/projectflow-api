#include "ActivityController.h"

#include "../repositories/ActivityRepository.h"
#include "../services/ActivityService.h"

#include <drogon/drogon.h>
#include <exception>
#include <string>

namespace
{
    drogon::HttpResponsePtr MakeErrorResponse(
        const std::string &code,
        const std::string &message,
        drogon::HttpStatusCode statusCode)
    {
        Json::Value body;
        body["error"]["code"] = code;
        body["error"]["message"] = message;

        auto response = drogon::HttpResponse::newHttpJsonResponse(body);
        response->setStatusCode(statusCode);

        return response;
    }

    Json::Value ActivityToJson(const ActivityLog &log)
    {
        Json::Value result;
        result["id"] = log.id;
        result["entity_type"] = log.entityType;
        result["entity_id"] = log.entityId;
        result["action"] = log.action;
        result["description"] = log.description;
        result["created_at"] = log.createdAt;

        return result;
    }
}

void ActivityController::GetActivity(
    const drogon::HttpRequestPtr &req,
    std::function<void(const drogon::HttpResponsePtr &)> &&callback)
{
    try
    {
        int limit = 20;
        int offset = 0;

        std::string limitParam = req->getParameter("limit");
        std::string offsetParam = req->getParameter("offset");

        if (!limitParam.empty())
        {
            try
            {
                limit = std::stoi(limitParam);
            }
            catch (...)
            {
                callback(MakeErrorResponse(
                    "INVALID_LIMIT",
                    "Limit must be a number between 1 and 100",
                    drogon::k400BadRequest));
                return;
            }
        }

        if (!offsetParam.empty())
        {
            try
            {
                offset = std::stoi(offsetParam);
            }
            catch (...)
            {
                callback(MakeErrorResponse(
                    "INVALID_OFFSET",
                    "Offset must be a non-negative number",
                    drogon::k400BadRequest));
                return;
            }
        }

        auto dbClient = drogon::app().getDbClient();

        ActivityRepository repository(dbClient);
        ActivityService service(repository);

        auto logs = service.GetRecentActivity(limit, offset);

        Json::Value result(Json::arrayValue);

        for (const auto &log : logs)
        {
            result.append(ActivityToJson(log));
        }

        auto response = drogon::HttpResponse::newHttpJsonResponse(result);
        response->setStatusCode(drogon::k200OK);

        callback(response);
    }
    catch (const std::exception &e)
    {
        std::string message = e.what();

        if (message == "Invalid limit")
        {
            callback(MakeErrorResponse(
                "INVALID_LIMIT",
                "Limit must be between 1 and 100",
                drogon::k400BadRequest));
            return;
        }

        if (message == "Invalid offset")
        {
            callback(MakeErrorResponse(
                "INVALID_OFFSET",
                "Offset must be a non-negative number",
                drogon::k400BadRequest));
            return;
        }

        callback(MakeErrorResponse(
            "INTERNAL_SERVER_ERROR",
            e.what(),
            drogon::k500InternalServerError));
    }
}