#include <drogon/drogon.h>

#include <cstdlib>
#include <iostream>
#include <stdexcept>

int main() {
    try {
        drogon::app().setLogLevel(trantor::Logger::kInfo);

        const char* dbPassword = std::getenv("TASK_MANAGER_DB_PASSWORD");

        if (dbPassword == nullptr) {
            std::cerr << "Environment variable TASK_MANAGER_DB_PASSWORD is not set" << std::endl;
            return 1;
        }

        drogon::app().createDbClient(
            "postgresql",
            "127.0.0.1",
            5432,
            "task_manager_db",
            "postgres",
            dbPassword,
            1
        );

        LOG_INFO << "Database client created";
        LOG_INFO << "Starting server on http://localhost:8080";

        drogon::app()
            .addListener("0.0.0.0", 8080)
            .run();

        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
}