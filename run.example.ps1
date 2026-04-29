$env:PROJECTFLOW_DB_HOST = "127.0.0.1"
$env:PROJECTFLOW_DB_PORT = "5432"
$env:PROJECTFLOW_DB_NAME = "task_manager_db"
$env:PROJECTFLOW_DB_USER = "postgres"
$env:PROJECTFLOW_DB_PASSWORD = "postgres"
$env:PROJECTFLOW_SERVER_PORT = "8080"

Set-Location "C:\Projects\Task Manager API\build"

cmake --build .

if ($LASTEXITCODE -ne 0) {
    Write-Host "Build failed. Server will not start."
    exit 1
}

.\Debug\task_manager.exe