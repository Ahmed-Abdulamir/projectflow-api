# ProjectFlow API

![C++](https://img.shields.io/badge/C%2B%2B-17-blue)
![Drogon](https://img.shields.io/badge/Drogon-Framework-green)
![PostgreSQL](https://img.shields.io/badge/PostgreSQL-Database-blue)
![CMake](https://img.shields.io/badge/CMake-Build-red)
![License](https://img.shields.io/badge/License-MIT-yellow)
![Project Check](https://github.com/Ahmed-Abdulamir/projectflow-api/actions/workflows/project-check.yml/badge.svg)

ProjectFlow API is a production-like RESTful backend service written in C++ using Drogon and PostgreSQL.

The project started as a task manager API and is being developed into a project management / issue tracking backend. It follows layered architecture and supports projects, tasks, comments, activity logs, PostgreSQL persistence, health checks, input validation, structured error responses, filtering, search, pagination, and sorting.

## Tech Stack

- C++17
- Drogon
- PostgreSQL
- CMake
- vcpkg
- Windows / PowerShell

## Features

- Projects CRUD
- Tasks CRUD
- Task comments
- Activity log
- Project-task relation
- Nested resources: project tasks and task comments
- Extended task model
- Health check endpoint
- Database health check endpoint
- PostgreSQL persistence
- Layered architecture
- Structured JSON error responses
- Environment-based database password configuration
- Filtering by status and priority
- Search by title and description
- Pagination with limit and offset
- Sorting by id, creation date, priority, and status

## Architecture

The project follows a layered architecture:

```text
Controller → Service → Repository → Database
```

Responsibilities:

```text
Controller  - handles HTTP requests and responses
Service     - contains business logic and validation
Repository  - works with PostgreSQL and SQL queries
Model       - stores data structures
```

## Project Structure

```text
ProjectFlow API/
├─ controllers/
│  ├─ TaskController.h
│  ├─ TaskController.cpp
│  ├─ ProjectController.h
│  ├─ ProjectController.cpp
│  ├─ CommentController.h
│  ├─ CommentController.cpp
│  ├─ ActivityController.h
│  └─ ActivityController.cpp
├─ services/
│  ├─ TaskService.h
│  ├─ TaskService.cpp
│  ├─ ProjectService.h
│  ├─ ProjectService.cpp
│  ├─ CommentService.h
│  ├─ CommentService.cpp
│  ├─ ActivityService.h
│  └─ ActivityService.cpp
├─ repositories/
│  ├─ TaskRepository.h
│  ├─ TaskRepository.cpp
│  ├─ ProjectRepository.h
│  ├─ ProjectRepository.cpp
│  ├─ CommentRepository.h
│  ├─ CommentRepository.cpp
│  ├─ ActivityRepository.h
│  └─ ActivityRepository.cpp
├─ models/
│  ├─ Task.h
│  ├─ Project.h
│  ├─ Comment.h
│  └─ ActivityLog.h
├─ database/
│  └─ schema.sql
├─ main.cpp
├─ CMakeLists.txt
├─ run.example.ps1
├─ .gitignore
└─ README.md
```

## Task Model

Current task representation:

```json
{
  "id": 13,
  "project_id": 1,
  "title": "Task inside project",
  "description": "Linked to project",
  "status": "todo",
  "priority": "high",
  "completed": false,
  "created_at": "2026-04-29T00:54:55",
  "updated_at": "2026-04-29T00:54:55"
}
```

Allowed task statuses:

```text
todo
in_progress
done
cancelled
```

Allowed task priorities:

```text
low
medium
high
```

## Project Model

```json
{
  "id": 1,
  "name": "Backend Internship Prep",
  "description": "Project for C++ backend portfolio",
  "created_at": "2026-04-28T23:50:05",
  "updated_at": "2026-04-28T23:58:53"
}
```

## Comment Model

```json
{
  "id": 1,
  "task_id": 13,
  "content": "This task belongs to project 1 and now has a comment",
  "created_at": "2026-04-29T01:20:03"
}
```

## Activity Log Model

```json
{
  "id": 10,
  "entity_type": "TASK",
  "entity_id": 15,
  "action": "TASK_DELETED",
  "description": "Task deleted",
  "created_at": "2026-04-29T01:44:57"
}
```

## Database Schema

The database schema is located in:

```text
database/schema.sql
```

Main tables:

- `projects`
- `tasks`
- `comments`
- `activity_logs`

The schema includes foreign keys for project-task and task-comment relations.

## API Endpoints

| Method | Endpoint | Description |
|---|---|---|
| GET | `/health` | Check if the server is running |
| GET | `/health/db` | Check database connection |
| GET | `/activity` | Get recent activity logs |
| POST | `/projects` | Create a project |
| GET | `/projects` | Get all projects |
| GET | `/projects/{id}` | Get project by ID |
| PATCH | `/projects/{id}` | Update project by ID |
| DELETE | `/projects/{id}` | Delete project by ID |
| GET | `/projects/{id}/tasks` | Get tasks for a project |
| POST | `/tasks` | Create a task |
| GET | `/tasks` | Get tasks with filtering, search, pagination, and sorting |
| GET | `/tasks/{id}` | Get task by ID |
| PATCH | `/tasks/{id}` | Update task by ID |
| DELETE | `/tasks/{id}` | Delete task by ID |
| POST | `/tasks/{id}/comments` | Create a comment for a task |
| GET | `/tasks/{id}/comments` | Get comments for a task |
| DELETE | `/comments/{id}` | Delete comment by ID |

## Query Parameters for `GET /tasks`

| Parameter | Description | Example |
|---|---|---|
| `status` | Filter by task status | `status=todo` |
| `priority` | Filter by priority | `priority=high` |
| `search` | Search in title and description | `search=validation` |
| `limit` | Number of tasks to return, from 1 to 100 | `limit=10` |
| `offset` | Number of tasks to skip | `offset=20` |
| `sort` | Sorting mode | `sort=created_at_desc` |

Supported sort values:

```text
id_desc
id_asc
created_at_desc
created_at_asc
priority_desc
priority_asc
status_asc
```

Example:

```text
GET /tasks?status=todo&priority=medium&search=api&limit=5&offset=0&sort=created_at_desc
```

## Activity Log

The API records important system events in the activity log.

Tracked events:

```text
PROJECT_CREATED
PROJECT_UPDATED
PROJECT_DELETED
TASK_CREATED
TASK_UPDATED
TASK_DELETED
COMMENT_CREATED
COMMENT_DELETED
```

Example request:

```powershell
Invoke-RestMethod -Uri "http://localhost:8080/activity?limit=10&offset=0" -Method GET
```

Example response:

```json
[
  {
    "id": 10,
    "entity_type": "TASK",
    "entity_id": 15,
    "action": "TASK_DELETED",
    "description": "Task deleted",
    "created_at": "2026-04-29T01:44:57"
  }
]
```

## Example Requests

### Health check

```powershell
Invoke-RestMethod -Uri "http://localhost:8080/health" -Method GET
```

Example response:

```json
{
  "service": "ProjectFlow API",
  "status": "ok"
}
```

### Database health check

```powershell
Invoke-RestMethod -Uri "http://localhost:8080/health/db" -Method GET
```

Example response:

```json
{
  "database": "connected",
  "status": "ok"
}
```

### Create project

```powershell
Invoke-RestMethod -Uri "http://localhost:8080/projects" `
  -Method POST `
  -ContentType "application/json" `
  -Body '{"name":"Backend Internship Prep","description":"Project for C++ backend portfolio"}'
```

### Get projects

```powershell
Invoke-RestMethod -Uri "http://localhost:8080/projects" -Method GET
```

### Get project by ID

```powershell
Invoke-RestMethod -Uri "http://localhost:8080/projects/1" -Method GET
```

### Update project

```powershell
Invoke-RestMethod -Uri "http://localhost:8080/projects/1" `
  -Method PATCH `
  -ContentType "application/json" `
  -Body '{"name":"Updated Backend Prep","description":"Updated project description"}'
```

### Delete project

```powershell
Invoke-RestMethod -Uri "http://localhost:8080/projects/1" -Method DELETE
```

### Create task with project_id

```powershell
Invoke-RestMethod -Uri "http://localhost:8080/tasks" `
  -Method POST `
  -ContentType "application/json" `
  -Body '{"project_id":1,"title":"Task inside project","description":"Linked to project","status":"todo","priority":"high"}'
```

### Create task without project_id

```powershell
Invoke-RestMethod -Uri "http://localhost:8080/tasks" `
  -Method POST `
  -ContentType "application/json" `
  -Body '{"title":"Learn Drogon"}'
```

### Get all tasks

```powershell
Invoke-RestMethod -Uri "http://localhost:8080/tasks" -Method GET
```

### Get tasks with filters, search, pagination, and sorting

```powershell
Invoke-RestMethod -Uri "http://localhost:8080/tasks?status=todo&priority=medium&search=task&limit=5&offset=0&sort=created_at_desc" -Method GET
```

### Get task by ID

```powershell
Invoke-RestMethod -Uri "http://localhost:8080/tasks/13" -Method GET
```

### Update task

```powershell
Invoke-RestMethod -Uri "http://localhost:8080/tasks/13" `
  -Method PATCH `
  -ContentType "application/json" `
  -Body '{"title":"Updated task","description":"Updated description","status":"done","priority":"medium","completed":true}'
```

### Delete task

```powershell
Invoke-RestMethod -Uri "http://localhost:8080/tasks/13" -Method DELETE
```

### Get tasks for project

```powershell
Invoke-RestMethod -Uri "http://localhost:8080/projects/1/tasks" -Method GET
```

### Create comment for task

```powershell
Invoke-RestMethod -Uri "http://localhost:8080/tasks/13/comments" `
  -Method POST `
  -ContentType "application/json" `
  -Body '{"content":"This task belongs to project 1 and now has a comment"}'
```

### Get comments for task

```powershell
Invoke-RestMethod -Uri "http://localhost:8080/tasks/13/comments" -Method GET
```

### Delete comment

```powershell
Invoke-RestMethod -Uri "http://localhost:8080/comments/1" -Method DELETE
```

## Error Format

The API uses structured JSON error responses:

```json
{
  "error": {
    "code": "TASK_NOT_FOUND",
    "message": "Task not found"
  }
}
```

Example error codes:

```text
INVALID_JSON
INVALID_TITLE
INVALID_DESCRIPTION
INVALID_STATUS
INVALID_PRIORITY
INVALID_COMPLETED
INVALID_LIMIT
INVALID_OFFSET
INVALID_SEARCH
INVALID_SORT
INVALID_PROJECT_ID
INVALID_PROJECT_NAME
INVALID_PROJECT_DESCRIPTION
INVALID_TASK_ID
INVALID_COMMENT_ID
INVALID_COMMENT_CONTENT
TASK_NOT_FOUND
PROJECT_NOT_FOUND
COMMENT_NOT_FOUND
DATABASE_CONNECTION_ERROR
INTERNAL_SERVER_ERROR
```

## Local Setup

### 1. Install dependencies

Required tools:

- CMake
- Visual Studio Build Tools
- vcpkg
- PostgreSQL

Install Drogon with PostgreSQL support:

```powershell
.\vcpkg.exe install drogon[postgres]:x64-windows --recurse
```

### 2. Create PostgreSQL database

Create a database named:

```text
task_manager_db
```

Then run the SQL from:

```text
database/schema.sql
```

### 3. Configure database password

Copy the example run script:

```powershell
Copy-Item run.example.ps1 run.ps1
```

Open `run.ps1` and replace:

```text
your_postgres_password
```

with your real local PostgreSQL password.

`run.ps1` is ignored by Git and must not be committed.

### 4. Build and run

From the project root:

```powershell
.\run.ps1
```

The server will start on:

```text
http://localhost:8080
```

## Current Status

Implemented:

- Projects CRUD
- Task CRUD
- Task comments
- Activity log
- Project-task relation
- Nested endpoints
- Extended task model
- Health checks
- PostgreSQL integration
- Structured errors
- Input validation
- Filtering
- Search
- Pagination
- Sorting
- Layered architecture
- Local run script
- Database schema file

Planned improvements:

- Users and authentication
- Unit and integration tests
- Docker Compose setup
- CI pipeline
- Database migrations

## Resume Description

ProjectFlow API is a production-like RESTful backend service written in C++ using Drogon and PostgreSQL. It implements projects, tasks, comments, activity logs, PostgreSQL persistence, layered architecture, input validation, structured error handling, filtering, search, pagination, sorting, nested resources, and CMake/vcpkg-based build configuration.

## Smoke Tests

The project includes a PowerShell smoke test script:

```powershell
.\tests\smoke_test.ps1

## Current Status

Implemented:

- Projects CRUD
- Task CRUD
- Task comments
- Activity log
- Project-task relation
- Nested endpoints
- Extended task model
- Health checks
- PostgreSQL integration
- Docker Compose for PostgreSQL
- Structured errors
- Input validation
- Filtering
- Search
- Pagination
- Sorting
- Layered architecture
- Local run script
- Database schema file
- Smoke test script
- GitHub Actions project check

Planned improvements:

- Users and authentication
- Unit and integration tests
- Full backend Docker image
- Database migrations

## Resume Description

ProjectFlow API is a production-like RESTful backend service written in C++ using Drogon and PostgreSQL. It implements projects, tasks, comments, activity logs, PostgreSQL persistence, layered architecture, input validation, structured error handling, filtering, search, pagination, sorting, nested resources, Docker Compose for PostgreSQL, smoke tests, GitHub Actions checks, and CMake/vcpkg-based build configuration.

## Smoke Tests

The project includes a PowerShell smoke test script:

```powershell
.\tests\smoke_test.ps1
```

The script checks health endpoints, project creation, task creation, project-task relation, comments, activity logs, and cleanup.

## License

This project is licensed under the MIT License.