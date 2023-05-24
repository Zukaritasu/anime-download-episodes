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

#pragma once

#include "framework.h"
#include <vector>
#include <iostream>

#define DB_ANIMES "animes"
#define DB_EPISODES "episodes"

struct AnimeData
{
	std::string name;
	std::string token;
	std::string description;
	std::vector<std::string> genres;
	int episode;
	int current;
	bool updated = false; // default false!
};

struct Episode
{
	std::string anime;
	std::string token;
	int         episode;
	int         number;
	bool        update; // update info anime
};

struct EpisodeData
{
	std::string token;
	int         number;
	std::string link;
	std::string date;
};

class SQLite
{
public:
	static bool addAnime(const wchar_t* token, int episode = 1);
	static bool addEpisode(const wchar_t* token, int episode, const wchar_t* link, 
		bool update_epi = true /* update number episode in animes */);
	static bool containsToken(const wchar_t* token);
	static bool deleteToken(const wchar_t* token);
	static bool loadDataBase(const char* sql_exists_table = nullptr, 
		const char* sql_create_table = nullptr);
	static bool updateAnimeInfo(const AnimeData& data);
	static bool updateAnimeInfo(std::string name, std::string description, 
		std::vector<std::string> genres, bool updated = true);
	static bool updateAnimeEpisode(const wchar_t* token, int episode);
	static bool exists(const char* query, sqlite3* db);
	static std::vector<AnimeData> getAnimeList();
	static std::vector<Episode> getEpisodesForDownload();
	static std::vector<EpisodeData> getEpisodes();

private:
	static sqlite3* open(const std::wstring& db = TEXT(DB_ANIMES));
	static bool createDirectoryDatabase();

	friend bool execute_query(const char* func, const char* query, bool free_query);

	SQLite() {}
};
