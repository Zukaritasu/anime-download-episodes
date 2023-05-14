#include "sqlmngr.h"
#include "utilities.h"

bool SQLite::addAnime(const wchar_t* token, int episode)
{
	if (loadDataBase())
	{

	}
	return false;
}

bool SQLite::containsToken(const wchar_t* token)
{
	if (loadDataBase())
	{

	}
	return true;
}

bool SQLite::deleteToken(const wchar_t* token)
{
	if (loadDataBase())
	{

	}
	return false;
}

bool SQLite::loadDataBase()
{
	if (!PathFileExists(L"db") && !CreateDirectory(L"db", nullptr))
	{
		_MSG_ERROR(NULL, L"Ha ocurrido un error al crear el directorio 'db'");
		return false;
	}

	sqlite3* db = nullptr;
	int rc = sqlite3_open16(L"db/animes.sqlite3", &db);
	if (rc == SQLITE_OK)
	{
		if (!tableExists("ANIMES", db))
		{
			sqlite3_exec(db, "CREATE TABLE ANIMES (VARCHAR, INT) ('name', 'episode-begin')", nullptr, nullptr, nullptr);
		}
	}
	else
	{
		_MSG_ERROR(NULL, std::wstring(L"Error al abrir la base de datos: ")
			.append((wchar_t*)sqlite3_errmsg16(db)).c_str());
	}
	sqlite3_close(db);
	return rc == SQLITE_OK;
}

bool SQLite::tableExists(const char* table, sqlite3* db)
{
	bool exists = false;
	char* msg = nullptr;
	std::string query = "SELECT name FROM sqlite_master WHERE type='table' AND name='";
	query.append(table).append("';");
	int rc = sqlite3_exec(db, query.c_str(), 
		[](void* data, int count, char** rows, char** columns)
		{
			*((bool*)data) = true; return SQLITE_OK;
		}, &exists, &msg);
	if (rc != SQLITE_OK)
	{
		printf("%s. %s\n", msg, sqlite3_db_filename(db, nullptr));
		sqlite3_free(msg);
	}
	return exists;
}

SQLite::SQLite()
{
}

