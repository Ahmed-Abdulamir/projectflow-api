# ProjectFlow API

ProjectFlow API is a RESTful backend service written in C++ using Drogon and PostgreSQL.

The project started as a task manager API and is being developed into a production-like project management backend. It follows layered architecture and provides CRUD operations for tasks, PostgreSQL persistence, health checks, input validation, structured error responses, filtering, search, pagination, and sorting.

## Tech Stack

- C++17
- Drogon
- PostgreSQL
- CMake
- vcpkg
- Windows / PowerShell

## Features

- Task CRUD
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
```

## Task Model

Current task representation:

```json
{
  "id": 1,
  "title": "Implement validation",
  "description": "Add validation for status and priority",
  "status": "in_progress",
  "priority": "high",
  "completed": false,
  "created_at": "2026-04-27T18:51:32",
  "updated_at": "2026-04-27T19:24:53"
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

## Database Schema

The database schema is located in:

```text
database/schema.sql
```

Current `tasks` table:

```sql
CREATE TABLE tasks (
    id SERIAL PRIMARY KEY,

    title TEXT NOT NULL,
    description TEXT NOT NULL DEFAULT '',

    status TEXT NOT NULL DEFAULT 'todo',
    priority TEXT NOT NULL DEFAULT 'medium',

    completed BOOLEAN NOT NULL DEFAULT FALSE,

    created_at TIMESTAMP NOT NULL DEFAULT NOW(),
    updated_at TIMESTAMP NOT NULL DEFAULT NOW(),

    CONSTRAINT tasks_status_check
        CHECK (status IN ('todo', 'in_progress', 'done', 'cancelled')),

    CONSTRAINT tasks_priority_check
        CHECK (priority IN ('low', 'medium', 'high'))
);
```

## API Endpoints

| Method | Endpoint | Description |
|---|---|---|
| GET | `/health` | Check if the server is running |
| GET | `/health/db` | Check database connection |
| POST | `/tasks` | Create a new task |
| GET | `/tasks` | Get tasks with filtering, search, pagination, and sorting |
| GET | `/tasks/{id}` | Get task by ID |
| PATCH | `/tasks/{id}` | Update task by ID |
| DELETE | `/tasks/{id}` | Delete task by ID |

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

### Create task with only title

```powershell
Invoke-RestMethod -Uri "http://localhost:8080/tasks" `
  -Method POST `
  -ContentType "application/json" `
  -Body '{"title":"Learn Drogon"}'
```

Example response:

```json
{
  "id": 1,
  "title": "Learn Drogon",
  "description": "",
  "status": "todo",
  "priority": "medium",
  "completed": false,
  "created_at": "2026-04-27T18:51:32",
  "updated_at": "2026-04-27T18:51:32"
}
```

### Create task with extended fields

```powershell
Invoke-RestMethod -Uri "http://localhost:8080/tasks" `
  -Method POST `
  -ContentType "application/json" `
  -Body '{"title":"Implement validation","description":"Add validation for status and priority","status":"in_progress","priority":"high"}'
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
Invoke-RestMethod -Uri "http://localhost:8080/tasks/1" -Method GET
```

### Update task

```powershell
Invoke-RestMethod -Uri "http://localhost:8080/tasks/1" `
  -Method PATCH `
  -ContentType "application/json" `
  -Body '{"title":"Updated task","description":"Updated description","status":"done","priority":"medium","completed":true}'
```

### Delete task

```powershell
Invoke-RestMethod -Uri "http://localhost:8080/tasks/1" -Method DELETE
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
TASK_NOT_FOUND
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

- Task CRUD
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

- Projects support
- Link tasks to projects
- Comments
- Activity log
- Users and authentication
- Unit and integration tests
- Docker Compose setup
- CI pipeline

## Resume Description

ProjectFlow API is a production-like RESTful backend service written in C++ using Drogon and PostgreSQL. It implements task CRUD operations, layered architecture, PostgreSQL persistence, health checks, input validation, structured error handling, filtering, search, pagination, sorting, and CMake/vcpkg-based build configuration.