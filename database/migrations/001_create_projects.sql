CREATE TABLE projects (
    id SERIAL PRIMARY KEY,

    name TEXT NOT NULL,
    description TEXT NOT NULL DEFAULT '',

    created_at TIMESTAMP NOT NULL DEFAULT NOW(),
    updated_at TIMESTAMP NOT NULL DEFAULT NOW()
);