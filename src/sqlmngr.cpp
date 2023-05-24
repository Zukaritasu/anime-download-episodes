/**
 * Copyright (C) 2023 Zukaritasu
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "sqlmngr.hpp"
#include "utilities.hpp"

#include <ctime>
#include <iostream>

#pragma warning(disable: 4996)

#define SQL_CREATE_TABLE_ANIME \
	"CREATE TABLE " DB_ANIMES \
	" (name TEXT, token TEXT, episode INTEGER, current INTEGER, updated INTEGER);"
#define SQL_INSERT_TABLE_ANIME \
	"INSERT INTO " DB_ANIMES \
	" (name, token, episode, current, updated) VALUES ('%s', '%s', %d, %d, %d);"
#define SQL_DELETE_TABLE_ANIME \
	"DELETE FROM " DB_ANIMES " WHERE token='%s';"
#define SQL_GET_TABLE_ANIME \
	"SELECT name, token, episode, current, updated FROM " DB_ANIMES ";"
#define SQL_SET_ANIME_EPISODE \
	"UPDATE " DB_ANIMES " SET current=%d WHERE token='%s';"

#define SQL_CREATE_TABLE_EPISODES \
	"CREATE TABLE " DB_EPISODES " (token TEXT, episode INTEGER, date INTEGER, link TEXT);"
#define SQL_INSERT_TABLE_EPISODES \
	"INSERT INTO " DB_EPISODES " (token, episode, date, link) VALUES ('%s', %d, %I64d, '%s');"
#define SQL_GET_EPISODES \
	"SELECT token, episode, date, link FROM " DB_EPISODES ";"

#define SQL_EXISTS_TABLE(table) \
	"SELECT name FROM sqlite_master WHERE type='table' AND name='" table "';"

#define _aux_value(param, value) if (param == nullptr) param = value;

static bool execute_query(const char* func, const char* query, 
	bool free_query = true);
static std::string get_date_time(__int64 date_time);


bool SQLite::addAnime(const wchar_t* token, int episode)
{
	if (loadDataBase())
	{
		char* query = sqlite3_mprintf(SQL_INSERT_TABLE_ANIME, wstring_to_string(token).c_str(),
			wstring_to_string(token).c_str(), episode, episode, 0);
		if (query != nullptr)
			return execute_query(__FUNCTION__ "()", query);
		_MSG_ERROR(NULL, L"Function sqlite3_mprintf() returns null.");
	}
	return false;
}

bool SQLite::addEpisode(const wchar_t* token, int episode, const wchar_t* link, bool update_epi)
{
	if (loadDataBase(SQL_EXISTS_TABLE(DB_EPISODES), SQL_CREATE_TABLE_EPISODES))
	{
		char* query = sqlite3_mprintf(SQL_INSERT_TABLE_EPISODES, wstring_to_string(token).c_str(),
				episode, std::time(nullptr), wstring_to_string(link).c_str());
		if (query != nullptr) {
			bool result = execute_query(__FUNCTION__ "()", query);
			if (result && update_epi)
				return updateAnimeEpisode(token, episode);
			return result;
		}
		_MSG_ERROR(NULL, L"Function sqlite3_mprintf() returns null.");
	}
	return false;
}

bool SQLite::containsToken(const wchar_t* token)
{
	if (loadDataBase())
	{
		std::string query = std::string("SELECT name FROM animes WHERE name='")
			.append(wstring_to_string(token)).append("';");
		return exists(query.c_str(), nullptr);
	}
	return false;
}

bool SQLite::deleteToken(const wchar_t* token)
{
	if (loadDataBase())
	{
		char* query = sqlite3_mprintf(SQL_DELETE_TABLE_ANIME, wstring_to_string(token).c_str());
		if (query != nullptr)
			return execute_query(__FUNCTION__ "()", query);
		_MSG_ERROR(NULL, L"Function sqlite3_mprintf() returns null.");
	}
	return false;
}

bool SQLite::loadDataBase(const char* sql_exists_table, const char* sql_create_table)
{
	_aux_value(sql_exists_table, SQL_EXISTS_TABLE(DB_ANIMES));
	_aux_value(sql_create_table, SQL_CREATE_TABLE_ANIME);

	sqlite3* db = SQLite::open();
	if (db != nullptr)
	{
		int rc = SQLITE_OK;
		if (!exists(sql_exists_table, db))
		{
			rc = sqlite3_exec(db, sql_create_table, NULL, NULL, NULL);
#ifdef _DEBUG
			printf(" > [DBG] " __FUNCTION__ "()" " %d\n", rc);
#endif // _DEBUG
			if (rc != SQLITE_OK)
				_MSG_ERROR(NULL, std::wstring(TEXT(__FUNCTION__ "()")
						" Error al ejecutar la sentencia SQL: ")
					.append((wchar_t*)sqlite3_errmsg16(db)).c_str());
		}
		sqlite3_close(db);
		return rc == SQLITE_OK;
	}
	return false;
}

bool SQLite::updateAnimeInfo(const AnimeData& data)
{

	return false;
}

bool SQLite::updateAnimeInfo(std::string name, std::string description, 
	std::vector<std::string> genres, bool updated)
{
	return false;
}

bool SQLite::updateAnimeEpisode(const wchar_t* token, int episode)
{
	if (loadDataBase())
	{
		return execute_query(__FUNCTION__ "()", format(SQL_SET_ANIME_EPISODE, 
			episode, TO_C_STR(token)).c_str(), false);
	}
	return false;
}

bool SQLite::exists(const char* query, sqlite3* db)
{
	bool is_open = false;
	if (db == nullptr)
	{
		if ((db = SQLite::open()) == nullptr) return false;
		is_open = true;
	}
		
	bool exists = false;
	char* msg = nullptr;
	int rc = sqlite3_exec(db, query, [](void* data, int count, char** rows, char** columns)
		{
			*((bool*)data) = true; return SQLITE_OK;
		}, &exists, &msg);
	if (rc != SQLITE_OK)
	{
		char* err_msg = sqlite3_mprintf("%s\n\nQuery:\n%s\n\n%s", msg, query, 
			sqlite3_db_filename(db, nullptr));
		if (err_msg != nullptr)
		{
			_AMSG_ERROR(nullptr, err_msg);
			sqlite3_free(err_msg);
		}
		sqlite3_free(msg);
	}
	if (is_open) sqlite3_close(db);
	return exists;
}

std::vector<AnimeData> SQLite::getAnimeList()
{
	if (!loadDataBase()) 
		return std::vector<AnimeData>();
	sqlite3* db = SQLite::open();
	if (db != nullptr)
	{
		std::vector<AnimeData> data;
		char* err_msg = nullptr;
		int rc = sqlite3_exec(db, SQL_GET_TABLE_ANIME, 
			[](void* data, int count, char** rows, char** columns)
			{
				assert(count == 5);
				reinterpret_cast<std::vector<AnimeData>*>(data)->push_back(
					{
						rows[0], rows[1], "", std::vector<std::string>(), 
						std::stoi(rows[2]), std::stoi(rows[3]),
						std::string(rows[4]) == "0"
					});
				return SQLITE_OK;
			}, 
		&data, &err_msg);
		if (rc != SQLITE_OK)
		{
#ifdef _DEBUG
			printf(" > [DBG] " __FUNCTION__ "() %s\n", err_msg);
#endif // _DEBUG
			_AMSG_ERROR(nullptr, err_msg);
			sqlite3_free(err_msg);
		}
		sqlite3_close(db);
		return data;
	}
	return std::vector<AnimeData>();
}

std::vector<Episode> SQLite::getEpisodesForDownload()
{
	std::vector<Episode> episodes;
	auto animes = getAnimeList();
	for (const auto& anime : animes)
	{
		episodes.push_back({ anime.name, anime.token, anime.episode, 
			anime.current == anime.episode ? anime.episode : anime.current + 1, 
			!anime.updated 
			}
		);
	}
	return episodes;
}

std::vector<EpisodeData> SQLite::getEpisodes()
{
	sqlite3* db = nullptr;
	if (!loadDataBase(SQL_EXISTS_TABLE(DB_EPISODES), SQL_CREATE_TABLE_EPISODES) ||
			(db = SQLite::open()) == nullptr)
		return std::vector<EpisodeData>();
	
	std::vector<EpisodeData> data;
	char* err_msg = nullptr;
	int rc = sqlite3_exec(db, SQL_GET_EPISODES,
		[](void* data, int count, char** rows, char** columns)
		{
			assert(count == 4);
			reinterpret_cast<std::vector<EpisodeData>*>(data)->push_back(
				{
					rows[0], std::stoi(rows[1]), get_date_time(std::stoll(rows[2])), rows[3]
				});
			return SQLITE_OK;
		},
		&data, &err_msg);
	if (rc != SQLITE_OK)
	{
		_AMSG_ERROR(nullptr, err_msg);
		sqlite3_free(err_msg);
	}
	sqlite3_close(db);
	return data;
}

sqlite3* SQLite::open(const std::wstring& dbname)
{
	if (!createDirectoryDatabase()) return nullptr;
	sqlite3* db = nullptr;
	int rc = sqlite3_open16(std::wstring(L"db/").append(dbname)
		.append(L".sqlite3").c_str(), &db);
	if (rc != SQLITE_OK)
	{
		_MSG_ERROR(NULL, std::wstring(L"Error al abrir la base de datos: ")
			.append((wchar_t*)sqlite3_errmsg16(db)).c_str());
		sqlite3_close(db);
		db = nullptr;
	}
	return db;
}

bool SQLite::createDirectoryDatabase()
{
	if (!PathFileExists(L"db") && !CreateDirectory(L"db", nullptr))
	{
		auto error_msg = get_last_error();
		if (!error_msg.empty())
			_MSG_ERROR(NULL, std::wstring(L"Error al crear el directorio 'db'. ")
				.append(error_msg).c_str());
		return false;
	}
	return true;
}

static bool execute_query(const char* func, const char* query, bool free_query)
{
	int rc = SQLITE_OK;
	sqlite3* db = SQLite::open();
	if (db == nullptr)
		rc = SQLITE_FAIL;
	else
	{
		char* _msg = nullptr;
		rc = sqlite3_exec(db, query, NULL, NULL, &_msg);
		if (rc != SQLITE_OK)
		{
			_AMSG_ERROR(NULL, std::string(func)
				.append(" Error al ejecutar la sentencia SQL: ")
				.append(_msg).c_str());
			sqlite3_free(_msg);
		}
		sqlite3_close(db);
	}
	if (free_query) sqlite3_free(const_cast<char*>(query));
	return rc == SQLITE_OK;
}

std::string get_date_time(__int64 date_time)
{
	char buffer[128] = { 0 };
	tm date_time_struct{};
	gmtime_s(&date_time_struct, &date_time);
	sprintf_s(buffer, "%2d/%2d/%4d", date_time_struct.tm_yday + 1, date_time_struct.tm_mon + 1, 
		date_time_struct.tm_year);
	return std::string(buffer);
}
