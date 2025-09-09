CREATE TABLE IF NOT EXISTS dictionary (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    word TEXT NOT NULL,
    pron TEXT,
    pos TEXT,
    translations TEXT,
    definition TEXT,
    example TEXT
);