cmake_minimum_required(VERSION 3.15)

project(ProjectFlowAPI)

set(CMAKE_CXX_STANDARD 17)

find_package(Drogon CONFIG REQUIRED)

file(GLOB SOURCES
    main.cpp
    controllers/TaskController.cpp
    controllers/ProjectController.cpp
    controllers/CommentController.cpp
    controllers/ActivityController.cpp
    services/TaskService.cpp
    services/ProjectService.cpp
    services/CommentService.cpp
    services/ActivityService.cpp
    repositories/TaskRepository.cpp
    repositories/ProjectRepository.cpp
    repositories/CommentRepository.cpp
    repositories/ActivityRepository.cpp
)

add_executable(task_manager ${SOURCES})

target_include_directories(task_manager PRIVATE ${CMAKE_SOURCE_DIR})

target_link_libraries(task_manager PRIVATE Drogon::Drogon)