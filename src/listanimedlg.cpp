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

#include "listanimedlg.hpp"
#include "resource.h"
#include "sqlmngr.hpp"
#include "utilities.hpp"
#include "tablectl.hpp"
#include "dialog.hpp"

#include <vector>

using namespace App::Dialogs;

class ListAnimeDialog : public Dialog
{
public:
	App::Controls::Table _table;

	ListAnimeDialog(unsigned int id);

	virtual void onInit() override;
	virtual void onSize() override;
	virtual void onInitComponents() override;
private:

};

void App::Dialogs::show_anime_list(HWND _hWnd) {
	(new ListAnimeDialog(IDD_LIST_ANIMES))->show(_hWnd);
}

ListAnimeDialog::ListAnimeDialog(unsigned int id) : Dialog(id) {}

void ListAnimeDialog::onInit()
{
	_table._hWnd = getControl(IDC_LIST_ANIMES);
	if (::IsThemeActive())
		::SetWindowTheme(_table, L"Explorer", nullptr);
	ListView_SetExtendedListViewStyle(_table, LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER);
}

void ListAnimeDialog::onSize()
{
	RECT rc;
	GetClientRect(_hWnd, &rc);
	MoveWindow(_table, 0, 0, rc.right, rc.bottom, TRUE);
}

void ListAnimeDialog::onInitComponents()
{
	_table.addColumns({
			{ L"Nombre del anime", 250 },
			{ L"Episodio",          80 },
			{ L"Episodio Actual",  100 },
		}
	);

	std::vector<AnimeData> data = SQLite::getAnimeList();
	for (const auto& row : data) {
		_table.addRow({ 
			row.name, std::to_string(row.episode), std::to_string(row.current) 
			}
		);
	}
}
