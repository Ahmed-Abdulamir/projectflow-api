#include <drogon/drogon.h>

#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>

namespace
{
    std::string GetEnvOrDefault(const char *name, const std::string &defaultValue)
    {
        const char *value = std::getenv(name);

        if (value == nullptr || std::string(value).empty())
        {
            return defaultValue;
        }

        return std::string(value);
    }

    std::string GetRequiredEnv(const char *name)
    {
        const char *value = std::getenv(name);

        if (value == nullptr || std::string(value).empty())
        {
            throw std::runtime_error(std::string("Required environment variable is not set: ") + name);
        }

        return std::string(value);
    }

    int GetEnvIntOrDefault(const char *name, int defaultValue)
    {
        const char *value = std::getenv(name);

        if (value == nullptr || std::string(value).empty())
        {
            return defaultValue;
        }

        try
        {
            return std::stoi(value);
        }
        catch (...)
        {
            throw std::runtime_error(std::string("Environment variable must be a number: ") + name);
        }
    }
}

int main()
{
    try
    {
        drogon::app().setLogLevel(trantor::Logger::kInfo);

        const std::string dbHost = GetEnvOrDefault("PROJECTFLOW_DB_HOST", "127.0.0.1");
        const int dbPort = GetEnvIntOrDefault("PROJECTFLOW_DB_PORT", 5432);
        const std::string dbName = GetEnvOrDefault("PROJECTFLOW_DB_NAME", "task_manager_db");
        const std::string dbUser = GetEnvOrDefault("PROJECTFLOW_DB_USER", "postgres");
        const std::string dbPassword = GetRequiredEnv("PROJECTFLOW_DB_PASSWORD");

        const int serverPort = GetEnvIntOrDefault("PROJECTFLOW_SERVER_PORT", 8080);

        drogon::app().createDbClient(
            "postgresql",
            dbHost,
            dbPort,
            dbName,
            dbUser,
            dbPassword,
            1);

        LOG_INFO << "Database client created";
        LOG_INFO << "Starting ProjectFlow API on http://localhost:" << serverPort;

        drogon::app()
            .addListener("0.0.0.0", serverPort)
            .run();

        return 0;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
}