-- Database Schema Definition
-- Demonstrates: DBMS (Table Design, Primary/Foreign Keys, Data Integrity)

CREATE TABLE IF NOT EXISTS users (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    username TEXT NOT NULL UNIQUE
);

CREATE TABLE IF NOT EXISTS messages (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    sender_id INTEGER NOT NULL,
    content TEXT NOT NULL,
    timestamp INTEGER NOT NULL,
    FOREIGN KEY(sender_id) REFERENCES users(id)
);

CREATE TABLE IF NOT EXISTS sessions (
    token TEXT PRIMARY KEY,
    user_id INTEGER NOT NULL,
    created_at INTEGER NOT NULL,
    FOREIGN KEY(user_id) REFERENCES users(id)
);
