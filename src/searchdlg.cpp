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

#include "searchdlg.hpp"
#include "utilities.hpp"
#include "sqlmngr.hpp"
#include "resource.h"
#include "curlconn.hpp"
#include "dialog.hpp"

#include <iostream>
#include <fstream>
#include <thread>
#include <nlohmann/json.hpp>

extern HINSTANCE hInst;

#define HANDLE_THREAD L"HANDLE_THREAD"
#define THREAD_PARAMS L"THREAD_PARAMS"

#define API_JSON_INVALID(_type, _property) \
	"La sintaxis del archivo " _type " JSON que retorno el servidor es invalido. " \
	"La propiedad '" _property "' no existe o no tiene un tipo de dato valido"

struct ThreadParams
{
	class SearchDialog* _dialog;
	HWND _hWnd = nullptr; /* Dialog */
	std::vector<Episode> episodes;
};

struct ResponseData
{
	std::string data;
	long httpCode;
};

class SearchDialog : public App::Dialogs::Dialog
{
public:
	SearchDialog(unsigned int id);

	virtual void onInit() override;
	virtual bool onCommand(int cmd, unsigned __int64 lParam) override;
	void freeResources();
	ResponseData getHTTPRequestData(std::string url);
	std::string getRequestURL(const Episode& episode);
	bool processResponseJSON(const Episode& episode, const nlohmann::json& json);
	void runThread();
	void validateAnimeJSON(const nlohmann::json& json);
	void saveJSON(const std::string& token, const nlohmann::json& json);
	void printLabel(const std::string& label);
	ThreadParams* createThreadParams();
	static DWORD run(LPVOID);
private:

};

namespace App {
	namespace Dialogs {
		void show_search_episodes(HWND _hWnd) {
			(new SearchDialog(IDD_SEARCH_EPISODES))->show(_hWnd);
		}
	}
}

SearchDialog::SearchDialog(unsigned int id) : Dialog(id) {}

void SearchDialog::onInit() 
{
	::SetFocus(getControl(IDCANCEL));
}

bool SearchDialog::onCommand(int cmd, unsigned __int64 lParam) 
{
	freeResources();
	if (cmd == IDCANCEL)
		return true;
	else if (cmd == IDOK) {
		try { 
			runThread();
		} catch (const std::exception& e) {
			_MSG_ERROR(_hWnd, ::string_to_wstring(e.what()).c_str());
			::EnableWindow(GetDlgItem(_hWnd, IDOK), TRUE);
		}
	}
	return false;
}

void SearchDialog::freeResources() 
{
	HANDLE handle = ::GetProp(_hWnd, HANDLE_THREAD);
	if (handle != nullptr) {
		::SetProp(_hWnd, HANDLE_THREAD, nullptr);
		::WaitForSingleObject(handle, IGNORE);
		::CloseHandle(handle);
	}
}

std::string SearchDialog::getRequestURL(const Episode& episode)
{
	return !episode.update ? format(API_URL PROVIDER "episode/%s-%d", 
				episode.token.c_str(), episode.number) : 
		format(API_URL PROVIDER "name/%s", episode.token.c_str());
}

ResponseData SearchDialog::getHTTPRequestData(std::string url)
{
	App::Connection::CurlConnection curl;
	curl.setURL(url);
	std::basic_string<char> buf;
	if (curl.readData(buf) && buf.size() > 0)
		return { buf, curl.getResponseCode() };
	return {};
}

DWORD SearchDialog::run(LPVOID lpVoid)
{
	ThreadParams* params = (ThreadParams*)lpVoid;
	SearchDialog* _dialog = params->_dialog;
	for (const auto& episode : params->episodes)
	{
		std::string url = _dialog->getRequestURL(episode);
		_dialog->printLabel(std::string("Buscando: ").append(url));
		ResponseData data = _dialog->getHTTPRequestData(url);
		if (!data.data.empty()) {
			if (!_dialog->processResponseJSON(episode, nlohmann::json::parse(data.data))) {
				break;
			}
		}
	}
	_dialog->printLabel("La busqueda a finalizado!");
	::EnableWindow(_dialog->getControl(IDOK), TRUE);
	return 0;
}

bool SearchDialog::processResponseJSON(const Episode& episode, 
	const nlohmann::json& json)
{
#ifdef _DEBUG
	std::cout << std::endl << json.dump() << std::endl;
#endif // _DEBUG
	try
	{
		if (episode.update) {
			validateAnimeJSON(json);
			saveJSON(episode.token, json); // fail save json?
			if (!SQLite::updateAnimeInfo(json["name"], json["synopsis"], json["genres"]))
				return false; // sql fail
			std::vector<nlohmann::json> episodes = json["episodes"];
			if (episode.number <= static_cast<int>(episodes.size())) {

			}
		} else {
			if (json.is_null() || !json.is_array())
				throw std::exception(API_JSON_INVALID("Episode", "/ estructura"));
			for (const auto& element : json.get<std::vector<nlohmann::json>>()) {
				if (element["name"].is_string() && equals_ignore_case(element["name"], "mega")) {
					if (element["file_url"].is_string())
						return SQLite::addEpisode(::TO_C_WSTR(episode.token),
							episode.number, ::TO_C_WSTR(element["file_url"]));
					break;
				}
			}
		}
	}
	catch (const std::exception& e)
	{
		::_MSG_ERROR(_hWnd, ::TO_C_WSTR(e.what()));
		return false;
	}
	return true;
}

void SearchDialog::printLabel(const std::string& label)
{
	::SendMessage(getControl(IDC_DLG_STATUS), WM_SETTEXT, 0,
		(LPARAM)::TO_C_WSTR(label));
}

void SearchDialog::runThread()
{
	::EnableWindow(getControl(IDOK), FALSE);
	HANDLE hThread = ::CreateThread(nullptr, 0, SearchDialog::run, 
		createThreadParams(), 0, 0);
	if (hThread == nullptr)
		throw std::exception(::wstring_to_string(::get_last_error()).c_str());
	::SetProp(_hWnd, HANDLE_THREAD, hThread);
}

ThreadParams* SearchDialog::createThreadParams() 
{
	std::vector<Episode> episodes = SQLite::getEpisodesForDownload();
	if (episodes.empty())
		throw std::exception("No hay animes disponibles para buscar sus nuevos episodios");
	return new ThreadParams{ this, _hWnd, episodes };
}

void SearchDialog::validateAnimeJSON(const nlohmann::json& json)
{
	if (json["genres"  ].is_null() || !json["genres"  ].is_array())
		throw std::exception(API_JSON_INVALID("Anime", "genres"));
	if (json["episodes"].is_null() || !json["episodes"].is_array())
		throw std::exception(API_JSON_INVALID("Anime", "episodes"));
	if (json["name"    ].is_null() || !json["name"    ].is_string())
		throw std::exception(API_JSON_INVALID("Anime", "name"));
	if (json["synopsis"].is_null() || !json["synopsis"].is_string())
		throw std::exception(API_JSON_INVALID("Anime", "synopsis"));
}

#define DIR_JSON_ANIMES "animes"

void SearchDialog::saveJSON(const std::string& token, const nlohmann::json& json)
{
	if (!PathFileExists(TEXT(DIR_JSON_ANIMES)) && 
		!CreateDirectory(TEXT(DIR_JSON_ANIMES), nullptr))
	{
		auto error_msg = wstring_to_string(::get_last_error());
		if (!error_msg.empty())
			throw std::exception(
				std::string("Error al crear el directorio '" DIR_JSON_ANIMES "'. ")
			.append(error_msg).c_str());
		return;
	}

	std::ofstream file_json("animes/" + token + ".json");
	if (file_json.is_open())
	{
		file_json << json;
		file_json.close();
	}
}