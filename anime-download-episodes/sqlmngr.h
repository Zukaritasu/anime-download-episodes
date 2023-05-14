#pragma once

#include "framework.h"

class SQLite
{
public:
	static bool addAnime(const wchar_t* token, int episode = 1);
	static bool containsToken(const wchar_t* token);
	static bool deleteToken(const wchar_t* token);
	static bool loadDataBase();
	static bool tableExists(const char* table, sqlite3* db);

private:
	SQLite();
};
