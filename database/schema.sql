DROP TABLE IF EXISTS tasks;

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