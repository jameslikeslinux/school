CREATE TABLE users (
	id INTEGER NOT NULL PRIMARY KEY,
	username VARCHAR NOT NULL,
	password VARCHAR NOT NULL,
	name VARCHAR NOT NULL
);

CREATE TABLE articles (
	id INTEGER NOT NULL PRIMARY KEY,
	uid INTEGER NOT NULL,
	datetime DATETIME NOT NULL,
	title VARCHAR,
	content TEXT
);

CREATE TABLE comments (
	id INTEGER NOT NULL PRIMARY KEY,
	aid INTEGER NOT NULL,
	uid INTEGER NOT NULL,
	datetime DATETIME NOT NULL,
	title VARCHAR,
	content TEXT
);

CREATE TABLE ignore (
	uid INTEGER NOT NULL,
	ignoreuid INTEGER NOT NULL
);