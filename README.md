# ProjectFlow API

ProjectFlow API is a RESTful backend service written in C++ using Drogon and PostgreSQL.

The project started as a task manager API and is being developed into a production-like project management backend. It follows layered architecture and provides CRUD operations for tasks, PostgreSQL persistence, health checks, input validation, and structured error responses.

## Tech Stack

- C++17
- Drogon
- PostgreSQL
- CMake
- vcpkg
- Windows / PowerShell

## Features

- Create a task
- Get all tasks
- Get a task by ID
- Update a task
- Delete a task
- Health check endpoint
- Database health check endpoint
- PostgreSQL persistence
- Layered architecture
- Structured JSON error responses
- Environment-based database password configuration

## Architecture

The project follows a layered architecture:

```text
Controller → Service → Repository → Database

Controller  - handles HTTP requests and responses
Service     - contains business logic and validation
Repository  - works with PostgreSQL and SQL queries
Model       - stores data structures

ProjectFlow API/
├─ controllers/
│  ├─ TaskController.h
│  └─ TaskController.cpp
├─ services/
│  ├─ TaskService.h
│  └─ TaskService.cpp
├─ repositories/
│  ├─ TaskRepository.h
│  └─ TaskRepository.cpp
├─ models/
│  └─ Task.h
├─ database/
│  └─ schema.sql
├─ main.cpp
├─ CMakeLists.txt
├─ run.example.ps1
├─ .gitignore
└─ README.md

description/status/priority/created_at/updated_at
фильтрацию
поиск
пагинацию
сортировку
примеры новых запросов