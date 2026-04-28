$env:TASK_MANAGER_DB_PASSWORD="your_postgres_password"

cd "C:\Projects\Task Manager API\build"

cmake --build .

if ($LASTEXITCODE -ne 0) {
    Write-Host "Build failed. Server will not start."
    exit 1
}

.\Debug\task_manager.exe