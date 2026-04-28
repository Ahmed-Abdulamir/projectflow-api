$ErrorActionPreference = "Stop"

$baseUrl = "http://localhost:8080"

Write-Host "Running ProjectFlow API smoke tests..."

Write-Host "Checking /health..."
$health = Invoke-RestMethod -Uri "$baseUrl/health" -Method GET
if ($health.status -ne "ok") {
    throw "Health check failed"
}

Write-Host "Checking /health/db..."
$dbHealth = Invoke-RestMethod -Uri "$baseUrl/health/db" -Method GET
if ($dbHealth.status -ne "ok" -or $dbHealth.database -ne "connected") {
    throw "Database health check failed"
}

Write-Host "Creating project..."
$project = Invoke-RestMethod -Uri "$baseUrl/projects" `
    -Method POST `
    -ContentType "application/json" `
    -Body '{"name":"Smoke Test Project","description":"Created by smoke test"}'

if (-not $project.id) {
    throw "Project creation failed"
}

Write-Host "Creating task..."
$taskBody = @{
    project_id = $project.id
    title = "Smoke Test Task"
    description = "Created by smoke test"
    status = "todo"
    priority = "high"
} | ConvertTo-Json

$task = Invoke-RestMethod -Uri "$baseUrl/tasks" `
    -Method POST `
    -ContentType "application/json" `
    -Body $taskBody

if (-not $task.id) {
    throw "Task creation failed"
}

Write-Host "Checking project tasks..."
$projectTasks = Invoke-RestMethod -Uri "$baseUrl/projects/$($project.id)/tasks" -Method GET
if ($projectTasks.Count -eq 0) {
    throw "Project tasks endpoint failed"
}

Write-Host "Creating comment..."
$comment = Invoke-RestMethod -Uri "$baseUrl/tasks/$($task.id)/comments" `
    -Method POST `
    -ContentType "application/json" `
    -Body '{"content":"Smoke test comment"}'

if (-not $comment.id) {
    throw "Comment creation failed"
}

Write-Host "Checking comments..."
$comments = Invoke-RestMethod -Uri "$baseUrl/tasks/$($task.id)/comments" -Method GET
if ($comments.Count -eq 0) {
    throw "Task comments endpoint failed"
}

Write-Host "Checking activity log..."
$activity = Invoke-RestMethod -Uri "$baseUrl/activity?limit=10&offset=0" -Method GET
if ($activity.Count -eq 0) {
    throw "Activity endpoint failed"
}

Write-Host "Cleaning up comment..."
Invoke-RestMethod -Uri "$baseUrl/comments/$($comment.id)" -Method DELETE | Out-Null

Write-Host "Cleaning up task..."
Invoke-RestMethod -Uri "$baseUrl/tasks/$($task.id)" -Method DELETE | Out-Null

Write-Host "Cleaning up project..."
Invoke-RestMethod -Uri "$baseUrl/projects/$($project.id)" -Method DELETE | Out-Null

Write-Host "Smoke tests passed successfully."